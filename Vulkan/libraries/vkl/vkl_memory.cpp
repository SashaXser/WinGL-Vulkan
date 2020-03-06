#include "vkl_memory.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <cstddef>
#include <ios>
#include <iostream>

namespace vkl
{

namespace device_memory
{

constexpr size_t DEVICE_INDEX = 1;
constexpr size_t PHYSICAL_DEVICE_INDEX = 2;
constexpr size_t SIZE_INDEX = 3;
constexpr size_t TYPE_INDEX_INDEX = 4;
constexpr size_t CONTEXT_SIZE = 5;

} // namespace device_memory

void DestroyDeviceMemoryHandle(
   const VkDeviceMemory * const memory )
{
   if (memory)
   {
      if (*memory)
      {
         vkFreeMemory(
            vkl::internal::GetContextData<
               VkDevice,
               device_memory::DEVICE_INDEX >(
                  memory),
            *memory,
            DefaultAllocator());
      }

      vkl::internal::DeallocateContext<
         device_memory::CONTEXT_SIZE >(
            memory);
   }
}

DeviceMemoryHandle SetAllocatedDeviceMemoryContextData(
   const vkl::internal::context_ptr_t context,
   const DeviceHandle & device,
   const VkDeviceSize size,
   const uint32_t type_index )
{
   namespace dm = device_memory;

   vkl::internal::SetContextData<
      dm::PHYSICAL_DEVICE_INDEX,
      dm::CONTEXT_SIZE >(
         context,
         GetPhysicalDevice(device));

   vkl::internal::SetContextData<
      dm::DEVICE_INDEX,
      dm::CONTEXT_SIZE >(
         context,
         *device);

    vkl::internal::SetContextData<
      dm::SIZE_INDEX,
      dm::CONTEXT_SIZE >(
         context,
         size);

   vkl::internal::SetContextData<
      dm::TYPE_INDEX_INDEX,
      dm::CONTEXT_SIZE >(
         context,
         type_index);

   return {
      vkl::internal::GetContextPointer<
         dm::CONTEXT_SIZE,
         VkDeviceMemory >(
            context),
      &DestroyDeviceMemoryHandle
   };
}

DeviceMemoryHandle AllocateDeviceMemory(
   const DeviceHandle & device,
   const VkDeviceSize size,
   const uint32_t type_index )
{
   DeviceMemoryHandle memory {
      nullptr, &DestroyDeviceMemoryHandle };

   if (device && *device)
   {
      namespace dm = device_memory;

      const auto context =
         vkl::internal::AllocateContext<
            dm::CONTEXT_SIZE >();

      if (context)
      {
         memory =
            SetAllocatedDeviceMemoryContextData(
               context,
               device,
               size,
               type_index);

         const VkMemoryAllocateInfo info {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            nullptr,
            size,
            type_index
         };

         const auto result =
            vkAllocateMemory(
               *device,
               &info,
               DefaultAllocator(),
               memory.get());

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Could not allocate device memory ("
               << result
               << ")!"
               << std::endl;

            memory.reset();
         }
         else
         {
            std::cout
               << "Allocated "
               << size / 1048576.0
               << " MiB of device memory!"
               << std::endl;
         }
      }
   }

   return memory;
}

VkDevice GetDevice(
   const DeviceMemoryHandle & memory )
{
   return
      vkl::internal::GetContextData<
         VkDevice,
         device_memory::DEVICE_INDEX >(
            memory.get());
}

VkPhysicalDevice GetPhysicalDevice(
   const DeviceMemoryHandle & memory )
{
   return
      vkl::internal::GetContextData<
         VkPhysicalDevice,
         device_memory::PHYSICAL_DEVICE_INDEX >(
            memory.get());
}

VkDeviceSize GetSize(
   const DeviceMemoryHandle & memory )
{
   return
      vkl::internal::GetContextData<
         VkDeviceSize,
         device_memory::SIZE_INDEX >(
            memory.get());
}

uint32_t GetTypeIndex(
   const DeviceMemoryHandle & memory )
{
   return
      vkl::internal::GetContextData<
         uint32_t,
         device_memory::TYPE_INDEX_INDEX >(
            memory.get());
}

namespace mapped_device_memory
{

constexpr size_t DEVICE_INDEX = 1;
constexpr size_t PHYSICAL_DEVICE_INDEX = 2;
constexpr size_t DEVICE_MEMORY_INDEX = 3;
constexpr size_t OFFSET_INDEX = 4;
constexpr size_t SIZE_INDEX = 5;
constexpr size_t MEM_MAP_FLAGS_INDEX = 6;
constexpr size_t CONTEXT_SIZE = 7;

} // namespace mapped_device_memory

void DestroyMappedDeviceMemoryHandle(
   const void * const * const mapped_memory )
{
   if (mapped_memory)
   {
      namespace mdm = mapped_device_memory;

      if (*mapped_memory)
      {
         vkUnmapMemory(
            vkl::internal::GetContextData<
               VkDevice,
               mdm::DEVICE_INDEX >(
                  mapped_memory),
            vkl::internal::GetContextData<
               VkDeviceMemory,
               mdm::DEVICE_MEMORY_INDEX >(
                  mapped_memory));
      }

      vkl::internal::DeallocateContext<
         mdm::CONTEXT_SIZE >(
            mapped_memory);
   }
}

MappedDeviceMemoryHandle SetMapDeviceMemoryContext(
   const vkl::internal::context_ptr_t context,
   const DeviceHandle & device,
   const DeviceMemoryHandle & device_memory,
   const VkDeviceSize offset,
   const VkDeviceSize size,
   const VkMemoryMapFlags flags )
{
   namespace mdm = mapped_device_memory;

   vkl::internal::SetContextData<
      mdm::DEVICE_MEMORY_INDEX,
      mdm::CONTEXT_SIZE >(
         context,
         *device_memory);

   vkl::internal::SetContextData<
      mdm::PHYSICAL_DEVICE_INDEX,
      mdm::CONTEXT_SIZE >(
         context,
         GetPhysicalDevice(device));

   vkl::internal::SetContextData<
      mdm::DEVICE_INDEX,
      mdm::CONTEXT_SIZE >(
         context,
         *device);

   vkl::internal::SetContextData<
      mdm::OFFSET_INDEX,
      mdm::CONTEXT_SIZE >(
         context,
         offset);

   vkl::internal::SetContextData<
      mdm::SIZE_INDEX,
      mdm::CONTEXT_SIZE >(
         context,
         size);

   vkl::internal::SetContextData<
      mdm::MEM_MAP_FLAGS_INDEX,
      mdm::CONTEXT_SIZE >(
         context,
         flags);

   return {
      vkl::internal::GetContextPointer<
         mdm::CONTEXT_SIZE,
         void * >(
            context),
      &DestroyMappedDeviceMemoryHandle
   };
}

MappedDeviceMemoryHandle MapDeviceMemory(
   const DeviceHandle & device,
   const DeviceMemoryHandle & device_memory,
   const VkDeviceSize offset,
   const VkDeviceSize size,
   const VkMemoryMapFlags flags )
{
   MappedDeviceMemoryHandle mapped_memory {
      nullptr, &DestroyMappedDeviceMemoryHandle };

   if (device && *device &&
       device_memory && *device_memory)
   {
      namespace mdm = mapped_device_memory;

      const auto context =
         vkl::internal::AllocateContext<
            mdm::CONTEXT_SIZE >();

      if (context)
      {
         mapped_memory =
            SetMapDeviceMemoryContext(
               context,
               device,
               device_memory,
               offset,
               size,
               flags);

         const auto result =
            vkMapMemory(
               *device,
               *device_memory,
               offset,
               size,
               flags,
               mapped_memory.get());

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Unable to map device memory ("
               << result
               << ")!"
               << std::endl;

            mapped_memory.reset();
         }
         else
         {
            std::cout
               << "Device memory mapped at 0x"
               << std::hex
               << *mapped_memory
               << std::dec
               << std::endl;
         }
      }
   }

   return mapped_memory;
}

VkDevice GetDevice(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData<
         VkDevice,
         mapped_device_memory::DEVICE_INDEX >(
            mapped_memory.get());
}

VkPhysicalDevice GetPhysicalDevice(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData<
         VkPhysicalDevice,
         mapped_device_memory::PHYSICAL_DEVICE_INDEX >(
            mapped_memory.get());
}

VkDeviceMemory GetDeviceMemory(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData<
         VkDeviceMemory,
         mapped_device_memory::DEVICE_MEMORY_INDEX >(
            mapped_memory.get());
}

VkDeviceSize GetOffset(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData<
         VkDeviceSize,
         mapped_device_memory::OFFSET_INDEX >(
            mapped_memory.get());
}

VkDeviceSize GetSize(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData<
         VkDeviceSize,
         mapped_device_memory::SIZE_INDEX >(
            mapped_memory.get());
}

VkMemoryMapFlags GetMemoryMapFlags(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData<
         VkMemoryMapFlags,
         mapped_device_memory::MEM_MAP_FLAGS_INDEX >(
            mapped_memory.get());
}

} // namespace vkl
