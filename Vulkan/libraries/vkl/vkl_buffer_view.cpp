#include "vkl_buffer_view.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>

namespace vkl
{

namespace
{

struct Context
{
   VkPhysicalDevice physical_device;
   DeviceHandle device;
   BufferHandle buffer;
   VkBufferViewCreateFlags create_flags;
   VkFormat format;
   VkDeviceSize offset;
   VkDeviceSize range;
};

} // namespace

void DestroyBufferViewHandle(
   const VkBufferView * const buffer_view )
{
   if (buffer_view)
   {
      if (*buffer_view)
      {
         const auto device =
            vkl::internal::GetContextData(
               buffer_view,
               &Context::device);

         if (device && *device)
         {
            vkDestroyBufferView(
               *device,
               *buffer_view,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         buffer_view);
   }
}

BufferViewHandle CreateBufferView(
   const DeviceHandle & device,
   const BufferHandle & buffer,
   const VkBufferViewCreateFlags create_flags,
   const VkFormat format,
   const VkDeviceSize offset,
   const VkDeviceSize range )
{
   BufferViewHandle buffer_view {
      nullptr, &DestroyBufferViewHandle };

   if (device && *device &&
       buffer && *buffer)
   {
      buffer_view.reset(
         vkl::internal::AllocateContext<
            VkBufferView,
            Context >(
               GetPhysicalDevice(device),
               device,
               buffer,
               create_flags,
               format,
               offset,
               range),
         &DestroyBufferViewHandle);

      if (buffer_view)
      {
         const VkBufferViewCreateInfo info {
            VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
            nullptr,
            create_flags,
            *buffer,
            format,
            offset,
            range
         };

         const auto result =
            vkCreateBufferView(
               *device,
               &info,
               DefaultAllocator(),
               buffer_view.get());

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Unable to create buffer view ("
               << result
               << ")!"
               << std::endl;

            buffer_view.reset();
         }
         else
         {
            std::cout
               << "Buffer view created!"
               << std::endl;
         }
      }
   }

   return buffer_view;
}

DeviceHandle GetDevice(
   const BufferViewHandle & buffer_view )
{
   return
      vkl::internal::GetContextData(
         buffer_view.get(),
         &Context::device);
}

VkPhysicalDevice GetPhysicalDevice(
   const BufferViewHandle & buffer_view )
{
   return
      vkl::internal::GetContextData(
         buffer_view.get(),
         &Context::physical_device);
}

} // namespace vkl
