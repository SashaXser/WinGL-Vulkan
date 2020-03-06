#ifndef _VKL_IMAGE_FWDS_H_
#define _VKL_IMAGE_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using ImageHandle =
   std::shared_ptr< VkImage >;

} // namespace vkl

#endif // _VKL_IMAGE_FWDS_H_
