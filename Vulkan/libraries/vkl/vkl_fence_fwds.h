#ifndef _VKL_FENCE_FWDS_H_
#define _VKL_FENCE_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using FenceHandle =
   std::shared_ptr< VkFence >;

} // namespace vkl

#endif // _VKL_FENCE_FWDS_H_
