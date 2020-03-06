#include "vkl_image_view.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <cstddef>
#include <iostream>

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

      vkl::internal::DeallocateContext<
         CONTEXT_SIZE >(
            image_view);
   }
}

ImageViewHandle SetImageViewContext(
   const vkl::internal::context_ptr_t context,
   const DeviceHandle & device,
   const ImageHandle & image,
   const VkImageViewCreateFlags create_flags,
   const VkImageViewType view_type,
   const VkFormat format,
   const VkComponentMapping & component_mapping,
   const VkImageSubresourceRange & subresource_range )
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

   return {
      vkl::internal::GetContextPointer<
         CONTEXT_SIZE,
         VkImageView >(
            context),
      &DestroyImageViewHandle
   };
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
      const auto context =
         vkl::internal::AllocateContext<
            CONTEXT_SIZE >();

      if (context)
      {
         image_view =
            SetImageViewContext(
               context,
               device,
               image,
               create_flags,
               view_type,
               format,
               component_mapping,
               subresource_range);

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

            image_view.reset();
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
