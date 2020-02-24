#include "vkl_image_view.h"
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

void DestroyImageViewHandle(
   const VkImageView * const image_view )
{
   if (image_view)
   {
      if (*image_view)
      {
         vkDestroyImageView(
            vkl::internal::GetContextData<
               VkDevice,
               DEVICE_INDEX >(
                  image_view),
            *image_view,
            DefaultAllocator());
      }

      delete []
         (reinterpret_cast< const size_t * >(
            image_view) - (CONTEXT_SIZE - 1));
   }
}

ImageViewHandle CreateImageView(
   const DeviceHandle & device,
   const ImageHandle & image,
   const VkImageViewCreateFlags create_flags,
   const VkImageViewType view_type,
   const VkFormat format,
   const VkComponentMapping & component_mapping,
   const VkImageSubresourceRange & subresource_range )
{
   ImageViewHandle image_view {
      nullptr, &DestroyImageViewHandle };

   if (device && *device &&
       image && *image)
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

         image_view.reset(
            reinterpret_cast< VkImageView * >(
               context + (CONTEXT_SIZE - 1)));

         const VkImageViewCreateInfo info {
            VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            nullptr,
            create_flags,
            *image,
            view_type,
            format,
            component_mapping,
            subresource_range
         };

         const auto result =
            vkCreateImageView(
               *device,
               &info,
               DefaultAllocator(),
               image_view.get());

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Image view not created successfully ("
               << result
               << ")!"
               << std::endl;

            image_view.reset(
               nullptr);
         }
         else
         {
            std::cout
               << "Image view created successfully!"
               << std::endl;

         }
      }
   }

   return image_view;
}

VkDevice GetDevice(
   const ImageViewHandle & image_view )
{
   return
      vkl::internal::GetContextData<
         VkDevice,
         DEVICE_INDEX >(
            image_view.get());
}

VkPhysicalDevice GetPhysicalDevice(
   const ImageViewHandle & image_view )
{
   return
      vkl::internal::GetContextData<
         VkPhysicalDevice,
         PHYSICAL_DEVICE_INDEX >(
            image_view.get());
}

} // namespace vkl