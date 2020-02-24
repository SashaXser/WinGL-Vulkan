#ifndef _VKL_COMMAND_BUFFER_FWDS_H_
#define _VKL_COMMAND_BUFFER_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using CommandBufferHandle =
   std::unique_ptr<
      VkCommandBuffer,
      void (*) ( const VkCommandBuffer * const ) >;

} // namespace vkl

#endif // _VKL_COMMAND_BUFFER_FWDS_H_
