#ifndef _VKL_IMAGE_VIEW_H_
#define _VKL_IMAGE_VIEW_H_

#include "vkl_device_fwds.h"
#include "vkl_image_fwds.h"
#include "vkl_image_view_fwds.h"

#include <vulkan/vulkan.h>

namespace vkl
{

ImageViewHandle CreateImageView(
   const DeviceHandle & device,
   const ImageHandle & image,
   const VkImageViewCreateFlags create_flags,
   const VkImageViewType view_type,
   const VkFormat format,
   const VkComponentMapping & component_mapping,
   const VkImageSubresourceRange & subresource_range );

DeviceHandle GetDevice(
   const ImageViewHandle & image_view );

VkPhysicalDevice GetPhysicalDevice(
   const ImageViewHandle & image_view );

} // namespace vkl

#endif // _VKL_IMAGE_VIEW_H_
