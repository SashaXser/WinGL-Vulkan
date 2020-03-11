#include "vkl_memory.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <ios>
#include <iostream>

namespace vkl
{

namespace device_memory
{

struct Context
{
   VkPhysicalDevice physical_device;
   DeviceHandle device;
   VkDeviceSize size;
   uint32_t type_index;
};

} // namespace device_memory

void DestroyDeviceMemoryHandle(
   const VkDeviceMemory * const memory )
{
   if (memory)
   {
      if (*memory)
      {
         const auto device =
            vkl::internal::GetContextData(
               memory,
               &device_memory::Context::device);

         if (device && *device)
         {
            vkFreeMemory(
               *device,
               *memory,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         memory);
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
      memory.reset(
         vkl::internal::AllocateContext<
            VkDeviceMemory,
            device_memory::Context >(
               GetPhysicalDevice(device),
               device,
               size,
               type_index),
         &DestroyDeviceMemoryHandle);

      if (memory)
      {
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

DeviceHandle GetDevice(
   const DeviceMemoryHandle & memory )
{
   return
      vkl::internal::GetContextData(
         memory.get(),
         &device_memory::Context::device);
}

VkPhysicalDevice GetPhysicalDevice(
   const DeviceMemoryHandle & memory )
{
   return
      vkl::internal::GetContextData(
         memory.get(),
         &device_memory::Context::physical_device);
}

VkDeviceSize GetSize(
   const DeviceMemoryHandle & memory )
{
   return
      vkl::internal::GetContextData(
         memory.get(),
         &device_memory::Context::size);
}

uint32_t GetTypeIndex(
   const DeviceMemoryHandle & memory )
{
   return
      vkl::internal::GetContextData(
         memory.get(),
         &device_memory::Context::type_index);
}

namespace mapped_device_memory
{

struct Context
{
   VkPhysicalDevice physical_device;
   DeviceHandle device;
   DeviceMemoryHandle device_memory;
   VkDeviceSize offset;
   VkDeviceSize size;
   VkMemoryMapFlags memory_map_flags;
};

} // namespace mapped_device_memory

void DestroyMappedDeviceMemoryHandle(
   const void * const * const mapped_memory )
{
   if (mapped_memory)
   {
      if (*mapped_memory)
      {
         const auto * const context =
            vkl::internal::GetContextData<
               const mapped_device_memory::Context >(
                  mapped_memory);

         if (context &&
             context->device && *context->device &&
             context->device_memory && *context->device_memory)
         {
            vkUnmapMemory(
               *context->device,
               *context->device_memory);
         }
      }

      vkl::internal::DeallocateContext(
         mapped_memory);
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
      mapped_memory.reset(
         vkl::internal::AllocateContext<
            void *,
            mapped_device_memory::Context >(
               GetPhysicalDevice(device),
               device,
               device_memory,
               offset,
               size,
               flags),
         &DestroyMappedDeviceMemoryHandle);

      if (mapped_memory)
      {
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

DeviceHandle GetDevice(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData(
         mapped_memory.get(),
         &mapped_device_memory::Context::device);
}

VkPhysicalDevice GetPhysicalDevice(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData(
         mapped_memory.get(),
         &mapped_device_memory::Context::physical_device);
}

DeviceMemoryHandle GetDeviceMemory(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData(
         mapped_memory.get(),
         &mapped_device_memory::Context::device_memory);
}

VkDeviceSize GetOffset(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData(
         mapped_memory.get(),
         &mapped_device_memory::Context::offset);
}

VkDeviceSize GetSize(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData(
         mapped_memory.get(),
         &mapped_device_memory::Context::size);
}

VkMemoryMapFlags GetMemoryMapFlags(
   const MappedDeviceMemoryHandle & mapped_memory )
{
   return
      vkl::internal::GetContextData(
         mapped_memory.get(),
         &mapped_device_memory::Context::memory_map_flags);
}

} // namespace vkl
