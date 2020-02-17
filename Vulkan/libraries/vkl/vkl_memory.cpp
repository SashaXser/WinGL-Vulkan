#include "vkl_memory.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <ios>
#include <iostream>
#include <new>

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

      delete []
         (reinterpret_cast< const size_t * >(
            memory) - (device_memory::CONTEXT_SIZE - 1));
   }
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

      size_t * const context =
         new (std::nothrow) size_t[dm::CONTEXT_SIZE] { };

      if (context)
      {
         *(context + (dm::CONTEXT_SIZE - dm::PHYSICAL_DEVICE_INDEX - 1)) =
            reinterpret_cast< size_t >(
               GetPhysicalDevice(device));

         *(context + (dm::CONTEXT_SIZE - dm::DEVICE_INDEX - 1)) =
            reinterpret_cast< size_t >(
               *device);

         *(context + (dm::CONTEXT_SIZE - dm::SIZE_INDEX - 1)) =
            size;

         *(context + (dm::CONTEXT_SIZE - dm::TYPE_INDEX_INDEX - 1)) =
            type_index;

         memory.reset(
            reinterpret_cast< VkDeviceMemory * >(
               context + (dm::CONTEXT_SIZE - 1)));

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

            memory.reset(
               nullptr);
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

      delete []
         (reinterpret_cast< const size_t * >(
            mapped_memory) - (mdm::CONTEXT_SIZE - 1));
   }
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

      size_t * const context =
         new (std::nothrow) size_t[mdm::CONTEXT_SIZE] { };

      if (context)
      {
         *(context + (mdm::CONTEXT_SIZE - mdm::DEVICE_MEMORY_INDEX - 1)) =
            reinterpret_cast< size_t >(
               *device_memory);

         *(context + (mdm::CONTEXT_SIZE - mdm::PHYSICAL_DEVICE_INDEX - 1)) =
            reinterpret_cast< size_t >(
               GetPhysicalDevice(device));

         *(context + (mdm::CONTEXT_SIZE - mdm::DEVICE_INDEX - 1)) =
            reinterpret_cast< size_t >(
               *device);

         *(context + (mdm::CONTEXT_SIZE - mdm::OFFSET_INDEX - 1)) =
            offset;

         *(context + (mdm::CONTEXT_SIZE - mdm::SIZE_INDEX - 1)) =
            size;

         *(context + (mdm::CONTEXT_SIZE - mdm::MEM_MAP_FLAGS_INDEX - 1)) =
            flags;

         mapped_memory.reset(
            reinterpret_cast< void ** >(
               context + (mdm::CONTEXT_SIZE - 1)));

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

            mapped_memory.reset(
               nullptr);
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
