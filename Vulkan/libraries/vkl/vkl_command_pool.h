#ifndef _VKL_COMMAND_POOL_H_
#define _VKL_COMMAND_POOL_H_

#include "vkl_device_fwds.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>

namespace vkl
{

using CommandPoolHandle =
   std::unique_ptr<
      VkCommandPool,
      void (*) ( const VkCommandPool * const ) >;

CommandPoolHandle CreateCommandPool(
   const DeviceHandle & device,
   const VkCommandPoolCreateFlags create_flags,
   const uint32_t queue_family_index );

bool ResetCommandPool(
   const CommandPoolHandle & command_pool,
   const VkCommandPoolResetFlags & reset_flags );

VkDevice GetDevice(
   const CommandPoolHandle & command_pool );

VkPhysicalDevice GetPhysicalDevice(
   const CommandPoolHandle & command_pool );

} // namespace vkl

#endif // _VKL_COMMAND_POOL_H_
