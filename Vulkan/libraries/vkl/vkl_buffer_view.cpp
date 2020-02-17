#include "vkl_buffer_view.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>
#include <new>

namespace vkl
{

constexpr size_t DEVICE_INDEX = 1;
constexpr size_t PHYSICAL_DEVICE_INDEX = 2;
constexpr size_t CONTEXT_SIZE = 3;

void DestroyBufferViewHandle(
   const VkBufferView * const buffer_view )
{
   if (buffer_view)
   {
      if (*buffer_view)
      {
         vkDestroyBufferView(
            vkl::internal::GetContextData<
               VkDevice,
               DEVICE_INDEX >(
                  buffer_view),
            *buffer_view,
            DefaultAllocator());
      }

      delete []
         (reinterpret_cast< const size_t * >(
            buffer_view) - (CONTEXT_SIZE - 1));
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

         buffer_view.reset(
            reinterpret_cast< VkBufferView * >(
               context + (CONTEXT_SIZE - 1)));

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

            buffer_view.reset(
               nullptr);
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

VkDevice GetDevice(
   const BufferViewHandle & buffer_view )
{
   return
      vkl::internal::GetContextData<
         VkDevice,
         DEVICE_INDEX >(
            buffer_view.get());
}

VkPhysicalDevice GetPhysicalDevice(
   const BufferViewHandle & buffer_view )
{
   return
      vkl::internal::GetContextData<
         VkPhysicalDevice,
         PHYSICAL_DEVICE_INDEX >(
            buffer_view.get());
}

} // namespace vkl
