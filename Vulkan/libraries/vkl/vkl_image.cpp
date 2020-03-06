#include "vkl_image.h"
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

void DestroyImageHandle(
   const VkImage * const image )
{
   if (image)
   {
      if (*image)
      {
         vkDestroyImage(
            vkl::internal::GetContextData<
               VkDevice,
               DEVICE_INDEX >(
                  image),
            *image,
            DefaultAllocator());
      }

      vkl::internal::DeallocateContext<
         CONTEXT_SIZE >(
            image);
   }
}

ImageHandle SetImageContext(
   const vkl::internal::context_ptr_t context,
   const DeviceHandle & device,
   const VkImageCreateFlags create_flags,
   const VkImageType image_type,
   const VkFormat image_format,
   const VkExtent3D image_extents,
   const uint32_t mip_levels,
   const uint32_t array_layers,
   const VkSampleCountFlagBits sample_count,
   const VkImageTiling image_tiling,
   const VkImageUsageFlags image_usage,
   const VkSharingMode sharing_mode,
   const VkImageLayout image_layout )
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
         VkImage >(
            context),
      &DestroyImageHandle
   };
}

ImageHandle CreateImage(
   const DeviceHandle & device,
   const VkImageCreateFlags create_flags,
   const VkImageType image_type,
   const VkFormat image_format,
   const VkExtent3D image_extents,
   const uint32_t mip_levels,
   const uint32_t array_layers,
   const VkSampleCountFlagBits sample_count,
   const VkImageTiling image_tiling,
   const VkImageUsageFlags image_usage,
   const VkSharingMode sharing_mode,
   const VkImageLayout image_layout )
{
   ImageHandle image {
      nullptr, &DestroyImageHandle };

   if (device && *device)
   {
      const auto context =
         vkl::internal::AllocateContext<
            CONTEXT_SIZE >();

      if (context)
      {
         image =
            SetImageContext(
               context,
               device,
               create_flags,
               image_type,
               image_format,
               image_extents,
               mip_levels,
               array_layers,
               sample_count,
               image_tiling,
               image_usage,
               sharing_mode,
               image_layout);

         const VkImageCreateInfo info {
            VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            nullptr,
            create_flags,
            image_type,
            image_format,
            image_extents,
            mip_levels,
            array_layers,
            sample_count,
            image_tiling,
            image_usage,
            sharing_mode,
            0, nullptr,
            image_layout
         };

         const auto result =
            vkCreateImage(
               *device,
               &info,
               DefaultAllocator(),
               image.get());

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Unable to create image ("
               << result
               << ")!"
               << std::endl;

            image.reset();
         }
         else
         {
            std::cout
               << "Image created successfully!"
               << std::endl;
         }
      }
   }

   return image;
}

VkDevice GetDevice(
   const ImageHandle & image )
{
   return
      vkl::internal::GetContextData<
         VkDevice,
         DEVICE_INDEX >(
            image.get());
}

VkPhysicalDevice GetPhysicalDevice(
   const ImageHandle & image )
{
   return
      vkl::internal::GetContextData<
         VkPhysicalDevice,
         PHYSICAL_DEVICE_INDEX >(
            image.get());
}

} // namespace vkl
