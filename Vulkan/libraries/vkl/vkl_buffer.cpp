#include "vkl_buffer.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>
#include <new>

namespace vkl
{

constexpr size_t DEVICE_INDEX = 1;
constexpr size_t PHYSICAL_DEVICE_INDEX = 2;
constexpr size_t SIZE_INDEX = 3;
constexpr size_t BUFFER_USAGE_FLAGS_INDEX = 4;
constexpr size_t SHARING_MODE_INDEX = 5;
constexpr size_t CONTEXT_SIZE = 6;

void DestroyBufferHandle(
   const VkBuffer * const buffer )
{
   if (buffer)
   {
      if (*buffer)
      {
         vkDestroyBuffer(
            vkl::internal::GetContextData<
               VkDevice,
               DEVICE_INDEX >(
                  buffer),
            *buffer,
            DefaultAllocator());
      }

      delete []
         (reinterpret_cast< const size_t * >(
            buffer) - (CONTEXT_SIZE - 1));
   }
}

BufferHandle CreateBuffer(
   const DeviceHandle & device,
   const VkDeviceSize size,
   const VkBufferUsageFlags usage,
   const VkSharingMode mode )
{
   BufferHandle buffer {
      nullptr,
      &DestroyBufferHandle };

   if (device && *device)
   {
      size_t * const context =
         new (std::nothrow) size_t[CONTEXT_SIZE] { };

      if (context)
      {
         *(context + (CONTEXT_SIZE - PHYSICAL_DEVICE_INDEX - 1)) =
            reinterpret_cast< size_t >(
               GetPhysicalDevice(device));

         *(context + (CONTEXT_SIZE - DEVICE_INDEX - 1)) =
            reinterpret_cast< size_t >(
               *device);

         *(context + (CONTEXT_SIZE - SIZE_INDEX - 1)) =
            size;

         *(context + (CONTEXT_SIZE - BUFFER_USAGE_FLAGS_INDEX - 1)) =
            usage;

         *(context + (CONTEXT_SIZE - SHARING_MODE_INDEX - 1)) =
            mode;

         buffer.reset(
            reinterpret_cast< VkBuffer * >(
               context + (CONTEXT_SIZE - 1)));

         const VkBufferCreateInfo info {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            nullptr,
            0, /* no sparse buffers */
            size,
            usage,
            mode,
            0, /* no queue family indices */
            nullptr /* no queue family indices */
         };

         const VkResult result =
            vkCreateBuffer(
               *device,
               &info,
               DefaultAllocator(),
               buffer.get());

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Unable to create buffer ("
               << result
               << ")!"
               << std::endl;

            buffer.reset(
               nullptr);
         }
         else
         {
            std::cout
               << "Buffer created of size "
               << size / 1048576.0
               << " MiB."
               << std::endl;
         }
      }
   }

   return buffer;
}

VkDevice GetDevice(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData<
         VkDevice,
         DEVICE_INDEX >(
            buffer.get());
}

VkPhysicalDevice GetPhysicalDevice(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData<
         VkPhysicalDevice,
         PHYSICAL_DEVICE_INDEX >(
            buffer.get());
}

VkDeviceSize GetSize(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData<
         VkDeviceSize,
         SIZE_INDEX >(
            buffer.get());
}

VkBufferUsageFlags GetBufferUsageFlags(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData<
         VkBufferUsageFlags,
         BUFFER_USAGE_FLAGS_INDEX >(
            buffer.get());
}

VkSharingMode GetSharingMode(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData<
         VkSharingMode,
         SHARING_MODE_INDEX >(
            buffer.get());
}

} // namespace vkl
