#ifndef _VKL_BUFFER_H_
#define _VKL_BUFFER_H_

#include "vkl_buffer_fwds.h"
#include "vkl_device_fwds.h"
#include "vkl_physical_device_fwds.h"

#include <vulkan/vulkan.h>

namespace vkl
{

BufferHandle CreateBuffer(
   const DeviceHandle & device,
   const VkDeviceSize size,
   const VkBufferUsageFlags usage,
   const VkSharingMode mode );

DeviceHandle GetDevice(
   const BufferHandle & buffer );

PhysicalDeviceHandle GetPhysicalDevice(
   const BufferHandle & buffer );

VkDeviceSize GetSize(
   const BufferHandle & buffer );

VkBufferUsageFlags GetBufferUsageFlags(
   const BufferHandle & buffer );

VkSharingMode GetSharingMode(
   const BufferHandle & buffer );

} // namespace vkl

#endif // _VKL_BUFFER_H_
