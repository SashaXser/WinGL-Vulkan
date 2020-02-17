#ifndef _VKL_IMAGE_VIEW_H_
#define _VKL_IMAGE_VIEW_H_

#include "vkl_device_fwds.h"
#include "vkl_image_fwds.h"

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using ImageViewHandle =
   std::unique_ptr<
      VkImageView,
      void (*) ( const VkImageView * const ) >;

ImageViewHandle CreateImageView(
   const DeviceHandle & device,
   const ImageHandle & image,
   const VkImageViewCreateFlags create_flags,
   const VkImageViewType view_type,
   const VkFormat format,
   const VkComponentMapping & component_mapping,
   const VkImageSubresourceRange & subresource_range );

VkDevice GetDevice(
   const ImageViewHandle & image_view );

VkPhysicalDevice GetPhysicalDevice(
   const ImageViewHandle & image_view );

} // namespace vkl

#endif // _VKL_IMAGE_VIEW_H_
