#ifndef _VKL_IMAGE_VIEW_FWDS_H_
#define _VKL_IMAGE_VIEW_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using ImageViewHandle =
   std::shared_ptr< VkImageView >;

} // namespace vkl

#endif // _VKL_IMAGE_VIEW_FWDS_H_
