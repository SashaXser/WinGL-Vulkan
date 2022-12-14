#ifndef _VKL_IMAGE_H_
#define _VKL_IMAGE_H_

#include "vkl_device_fwds.h"
#include "vkl_image_fwds.h"
#include "vkl_physical_device_fwds.h"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace vkl
{

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
   const VkImageLayout image_layout );

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
   const VkImageLayout image_layout );

DeviceHandle GetDevice(
   const ImageHandle & image );

PhysicalDeviceHandle GetPhysicalDevice(
   const ImageHandle & image );

} // namespace vkl

#endif // _VKL_IMAGE_H_
