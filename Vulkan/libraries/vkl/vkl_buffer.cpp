#include "vkl_buffer.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <cstddef>
#include <iostream>

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

      vkl::internal::DeallocateContext<
         CONTEXT_SIZE >(
            buffer);
   }
}

BufferHandle SetBufferContext(
   const vkl::internal::context_ptr_t context,
   const DeviceHandle & device,
   const VkDeviceSize size,
   const VkBufferUsageFlags usage,
   const VkSharingMode mode )
{
   vkl::internal::SetContextData<
      PHYSICAL_DEVICE_INDEX,
      CONTEXT_SIZE >(
         context,
         GetPhysicalDevice(device));

   vkl::internal::SetContextData<
      DEVICE_INDEX,
      CONTEXT_SIZE >(
         context,
         *device);

   vkl::internal::SetContextData<
      SIZE_INDEX,
      CONTEXT_SIZE >(
         context,
         size);

   vkl::internal::SetContextData<
      BUFFER_USAGE_FLAGS_INDEX,
      CONTEXT_SIZE >(
         context,
         usage);

   vkl::internal::SetContextData<
      SHARING_MODE_INDEX,
      CONTEXT_SIZE >(
         context,
         mode);

   return {
      vkl::internal::GetContextPointer<
         CONTEXT_SIZE,
         VkBuffer >(
            context),
      &DestroyBufferHandle
   };
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
      const auto context =
         vkl::internal::AllocateContext<
            CONTEXT_SIZE >();

      if (context)
      {
         buffer =
            SetBufferContext(
               context,
               device,
               size,
               usage,
               mode);

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
