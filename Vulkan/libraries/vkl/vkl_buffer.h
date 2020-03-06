#ifndef _VKL_BUFFER_H_
#define _VKL_BUFFER_H_

#include "vkl_device_fwds.h"

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using BufferHandle =
   std::shared_ptr< VkBuffer >;

BufferHandle CreateBuffer(
   const DeviceHandle & device,
   const VkDeviceSize size,
   const VkBufferUsageFlags usage,
   const VkSharingMode mode );

VkDevice GetDevice(
   const BufferHandle & buffer );

VkPhysicalDevice GetPhysicalDevice(
   const BufferHandle & buffer );

VkDeviceSize GetSize(
   const BufferHandle & buffer );

VkBufferUsageFlags GetBufferUsageFlags(
   const BufferHandle & buffer );

VkSharingMode GetSharingMode(
   const BufferHandle & buffer );

} // namespace vkl

#endif // _VKL_BUFFER_H_
