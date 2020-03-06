#ifndef _VKL_COMMAND_BUFFER_FWDS_H_
#define _VKL_COMMAND_BUFFER_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace vkl
{

using CommandBufferHandle =
   std::shared_ptr< VkCommandBuffer >;

using AllocatedCommandBuffers =
   std::vector< CommandBufferHandle >;

} // namespace vkl

#endif // _VKL_COMMAND_BUFFER_FWDS_H_
