#include "vkl_buffer.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>

namespace vkl
{

struct Context final
{
   VkPhysicalDevice physical_device;
   DeviceHandle device;
   VkDeviceSize size;
   VkBufferUsageFlags usage_flags;
   VkSharingMode sharing_mode;
};

void DestroyBufferHandle(
   const VkBuffer * const buffer )
{
   if (buffer)
   {
      if (*buffer)
      {
         const auto device =
            vkl::internal::GetContextData(
               buffer,
               &Context::device);

         if (device && *device)
         {
            vkDestroyBuffer(
               *device,
               *buffer,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         buffer);
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
      buffer.reset(
         vkl::internal::AllocateContext<
            VkBuffer,
            Context >(
               GetPhysicalDevice(device),
               device,
               size,
               usage,
               mode),
         &DestroyBufferHandle);

      if (buffer)
      {
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

            buffer.reset();
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

DeviceHandle GetDevice(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData(
         buffer.get(),
         &Context::device);
}

VkPhysicalDevice GetPhysicalDevice(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData(
         buffer.get(),
         &Context::physical_device);
}

VkDeviceSize GetSize(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData(
         buffer.get(),
         &Context::size);
}

VkBufferUsageFlags GetBufferUsageFlags(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData(
         buffer.get(),
         &Context::usage_flags);
}

VkSharingMode GetSharingMode(
   const BufferHandle & buffer )
{
   return
      vkl::internal::GetContextData(
         buffer.get(),
         &Context::sharing_mode);
}

} // namespace vkl
