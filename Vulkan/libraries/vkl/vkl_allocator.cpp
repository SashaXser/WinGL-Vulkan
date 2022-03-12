#include "vkl_allocator.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ios>
#include <iostream>
#include <mutex>
#include <new>

namespace vkl
{

const char * DecodeScope(
   const VkSystemAllocationScope scope )
{
   const char * scope_name = "unknown";

   switch (scope)
   {
   case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND: scope_name = "command"; break;
   case VK_SYSTEM_ALLOCATION_SCOPE_OBJECT: scope_name = "object"; break;
   case VK_SYSTEM_ALLOCATION_SCOPE_CACHE: scope_name = "cache"; break;
   case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE: scope_name = "device"; break;
   case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE: scope_name = "instance"; break;
   }

   return scope_name;
}

const char * DecodeType(
   const VkInternalAllocationType type )
{
   const char * type_name = "unknown";

   switch (type)
   {
   case VK_INTERNAL_ALLOCATION_TYPE_EXECUTABLE: type_name = "executable"; break;
   }

   return type_name;
}

namespace default
{

uint64_t allocated_size_ { };
std::mutex allocator_lock_ { };

void VKAPI_CALL Free(
   void * const /*user_data*/,
   void * const original_data )
{
   if (original_data)
   {
      size_t * const data =
         reinterpret_cast< size_t * >(original_data);

      size_t * const header =
         reinterpret_cast< size_t * >(*(data - 1));

      const size_t alignment = *(data - 2);

      const size_t size = *(data - 3);

      ::operator delete(
         header,
         std::align_val_t { alignment });

      std::lock_guard< std::mutex > lock {
         allocator_lock_ };

      std::cout
         << "Freeing " << size
         << " bytes with alignment "
         << alignment
         << ".  Allocated "
         << (allocated_size_ -= size) / 1048576.0
         << " MiB (0x"
         << std::hex << data << ")" << std::dec
         << std::endl;
   }
}

void * VKAPI_CALL Allocate(
   void * const /*user_data*/,
   const size_t size,
   const size_t alignment,
   const VkSystemAllocationScope scope )
{
   // expected to be a power of two
   assert(
      alignment != 0 &&
      (alignment & (alignment - 1)) == 0);

   const double ratio =
      24.0 / alignment;
   const size_t header_size =
      ratio > 1.0 ?
      uint32_t(ratio + 1) * alignment :
      alignment;

   uint8_t * const header =
      reinterpret_cast< uint8_t * >(
         ::operator new(
            size + header_size,
            std::align_val_t { alignment }));

   size_t * const data =
      reinterpret_cast< size_t * >(
         header + header_size);

   *(data - 1) =
      reinterpret_cast< size_t >(header);

   *(data - 2) = alignment;

   *(data - 3) = size;

   std::lock_guard< std::mutex > lock {
      allocator_lock_ };

   std::cout
      << "Allocating " << size
      << " bytes with alignment "
      << alignment << " and scope "
      << DecodeScope(scope)
      << ".  Allocated "
      << (allocated_size_ += size) / 1048576.0
      << " MiB (0x"
      << std::hex << data << ")" << std::dec
      << std::endl;

   return data;
}

void * VKAPI_CALL Reallocate(
   void * const user_data,
   void * const original_data,
   const size_t size,
   const size_t alignment,
   const VkSystemAllocationScope scope )
{
   void * data = nullptr;

   if (!original_data)
      data = Allocate(user_data, size, alignment, scope);
   else if (size == 0)
      Free(user_data, original_data);
   else
   {
      // there are other rules but lets assume they are satisfied
      data = Allocate(user_data, size, alignment, scope);

      const size_t original_size =
         *(reinterpret_cast< size_t * >(original_data) - 3);

      std::memcpy(
         data,
         original_data,
         std::min(original_size, size));

      Free(user_data, original_data);
   }

   return data;
}

void VKAPI_CALL InternalAllocate(
   void * const /*user_data*/,
   const size_t size,
   const VkInternalAllocationType type,
   const VkSystemAllocationScope scope )
{
   std::lock_guard< std::mutex > lock {
      allocator_lock_ };

   std::cout
      << "Internally Allocating "
      << size
      << " bytes with scope "
      << DecodeScope(scope)
      << " and type "
      << DecodeType(type)
      << ".  Allocated "
      << (allocated_size_ += size) / 1048576.0
      << " MiB"
      << std::endl;
}

void VKAPI_CALL InternalFree(
   void * const /*user_data*/,
   const size_t size,
   const VkInternalAllocationType type,
   const VkSystemAllocationScope scope )
{
   std::lock_guard< std::mutex > lock {
      allocator_lock_ };

   std::cout
      << "Internally Freeing "
      << size
      << " bytes with scope "
      << DecodeScope(scope)
      << " and type "
      << DecodeType(type)
      << ".  Allocated "
      << (allocated_size_ -= size) / 1048576.0
      << " MiB"
      << std::endl;
}

const VkAllocationCallbacks allocator_callbacks_ =
   [ ] ( )
   {
      VkAllocationCallbacks callbacks { };

      callbacks.pUserData = nullptr;
      callbacks.pfnAllocation = &Allocate;
      callbacks.pfnReallocation = &Reallocate;
      callbacks.pfnFree = &Free;
      callbacks.pfnInternalAllocation = &InternalAllocate;
      callbacks.pfnInternalFree = &InternalFree;

      return callbacks;
   } ();

} // namespace default

const VkAllocationCallbacks * DefaultAllocator( )
{
#ifndef VKL_DISABLE_DEFAULT_VULKAN_ALLOCATOR
   return &default::allocator_callbacks_;
#else
   return nullptr;
#endif
}

} // namespace vkl
