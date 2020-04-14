#include "vkl_image.h"
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
   VkImageCreateFlags create_flags;
   VkImageType image_type;
   VkFormat image_format;
   VkExtent3D image_extents;
   uint32_t mip_levels;
   uint32_t array_layers;
   VkSampleCountFlagBits sample_count;
   VkImageTiling image_tiling;
   VkImageUsageFlags image_usage;
   VkSharingMode sharing_mode;
   VkImageLayout image_layout;
};

} // namespace

void DestroyImageHandle(
   const VkImage * const image )
{
   if (image)
   {
      if (*image)
      {
         const auto device =
            vkl::internal::GetContextData(
               image,
               &Context::device);

         if (device && *device)
         {
            vkDestroyImage(
               *device,
               *image,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         image);
   }
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
      image.reset(
         vkl::internal::AllocateContext<
            VkImage,
            Context >(
               GetPhysicalDevice(device),
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
               image_layout),
         &DestroyImageHandle);

      if (image)
      {
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

ImageHandle CreateImage(
   const VkImage vk_image,
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
   ImageHandle image { nullptr };

   if (vk_image &&
       device && *device)
   {
      image.reset(
         vkl::internal::AllocateContext<
            VkImage,
            Context >(
               GetPhysicalDevice(device),
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
               image_layout),
         &vkl::internal::DeallocateContext);

      if (image)
      {
         *image = vk_image;
      }
   }

   return image;
}

DeviceHandle GetDevice(
   const ImageHandle & image )
{
   return
      vkl::internal::GetContextData(
         image.get(),
         &Context::device);
}

PhysicalDeviceHandle GetPhysicalDevice(
   const ImageHandle & image )
{
   return
      vkl::internal::GetContextData(
         image.get(),
         &Context::physical_device);
}

} // namespace vkl
