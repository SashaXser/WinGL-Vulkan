#include "vkl_image_view.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>

namespace vkl
{

namespace
{

struct Context final
{
   PhysicalDeviceHandle physical_device;
   DeviceHandle device;
   ImageHandle image;
   VkImageViewCreateFlags create_flags;
   VkImageViewType view_type;
   VkFormat format;
   VkComponentMapping component_mapping;
   VkImageSubresourceRange subresource_range;
};

} // namespace

void DestroyImageViewHandle(
   const VkImageView * const image_view )
{
   if (image_view)
   {
      if (*image_view)
      {
         const auto device =
            vkl::internal::GetContextData(
               image_view,
               &Context::device);

         if (device && *device)
         {
            vkDestroyImageView(
               *device,
               *image_view,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         image_view);
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
      image_view.reset(
         vkl::internal::AllocateContext<
            VkImageView,
            Context >(
               GetPhysicalDevice(device),
               device,
               image,
               create_flags,
               view_type,
               format,
               component_mapping,
               subresource_range),
         &DestroyImageViewHandle);

      if (image_view)
      {
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

DeviceHandle GetDevice(
   const ImageViewHandle & image_view )
{
   return
      vkl::internal::GetContextData(
         image_view.get(),
         &Context::device);
}

PhysicalDeviceHandle GetPhysicalDevice(
   const ImageViewHandle & image_view )
{
   return
      vkl::internal::GetContextData(
         image_view.get(),
         &Context::physical_device);
}

} // namespace vkl
