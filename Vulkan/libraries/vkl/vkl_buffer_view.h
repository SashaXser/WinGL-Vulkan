#ifndef _VKL_BUFFER_VIEW_H_
#define _VKL_BUFFER_VIEW_H_

#include "vkl_buffer_fwds.h"
#include "vkl_buffer_view_fwds.h"
#include "vkl_device_fwds.h"

#include <vulkan/vulkan.h>

namespace vkl
{

BufferViewHandle CreateBufferView(
   const DeviceHandle & device,
   const BufferHandle & buffer,
   const VkBufferViewCreateFlags create_flags,
   const VkFormat format,
   const VkDeviceSize offset,
   const VkDeviceSize range );

VkDevice GetDevice(
   const BufferViewHandle & buffer_view );

VkPhysicalDevice GetPhysicalDevice(
   const BufferViewHandle & buffer_view );

} // namespace vkl

#endif // _VKL_BUFFER_VIEW_H_
