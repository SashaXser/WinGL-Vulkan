#ifndef _VKL_COMMAND_POOL_H_
#define _VKL_COMMAND_POOL_H_

#include "vkl_command_pool_fwds.h"
#include "vkl_device_fwds.h"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace vkl
{

CommandPoolHandle CreateCommandPool(
   const DeviceHandle & device,
   const VkCommandPoolCreateFlags create_flags,
   const uint32_t queue_family_index );

bool ResetCommandPool(
   const CommandPoolHandle & command_pool,
   const VkCommandPoolResetFlags & reset_flags );

DeviceHandle GetDevice(
   const CommandPoolHandle & command_pool );

VkPhysicalDevice GetPhysicalDevice(
   const CommandPoolHandle & command_pool );

} // namespace vkl

#endif // _VKL_COMMAND_POOL_H_
