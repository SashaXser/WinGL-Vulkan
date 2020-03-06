#ifndef _VKL_COMMAND_POOL_FWDS_H_
#define _VKL_COMMAND_POOL_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using CommandPoolHandle =
   std::shared_ptr< VkCommandPool >;

} // namespace vkl

#endif // _VKL_COMMAND_POOL_FWDS_H_
