#ifndef _VKL_COMMAND_BUFFER_H_
#define _VKL_COMMAND_BUFFER_H_

#include "vkl_command_buffer_fwds.h"
#include "vkl_command_pool_fwds.h"
#include "vkl_device_fwds.h"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace vkl
{

CommandBufferHandle AllocateCommandBuffer(
   const DeviceHandle & device,
   const CommandPoolHandle & command_pool,
   const VkCommandBufferLevel command_buffer_level );

AllocatedCommandBuffers
AllocateCommandBuffers(
   const DeviceHandle & device,
   const CommandPoolHandle & command_pool,
   const VkCommandBufferLevel command_buffer_level,
   const uint32_t command_buffer_count );

bool BeginCommandBuffer(
   const CommandBufferHandle & command_buffer,
   const VkCommandBufferUsageFlags command_buffer_usage_flags );

bool EndCommandBuffer(
   const CommandBufferHandle & command_buffer );

VkDevice GetDevice(
   const CommandBufferHandle & command_buffer );

VkPhysicalDevice GetPhysicalDevice(
   const CommandBufferHandle & command_buffer );

} // namespace vkl

#endif // _VKL_COMMAND_BUFFER_H_
