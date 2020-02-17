#ifndef _VKL_IMAGE_FWDS_H_
#define _VKL_IMAGE_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using ImageHandle =
   std::unique_ptr<
      VkImage,
      void (*) ( const VkImage * const ) >;

} // namespace vkl

#endif // _VKL_IMAGE_FWDS_H_
