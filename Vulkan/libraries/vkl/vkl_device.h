#ifndef _VKL_DEVICE_H_
#define _VKL_DEVICE_H_

#include "vkl_device_fwds.h"
#include "vkl_physical_device_fwds.h"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace vkl
{

DeviceHandle CreateDevice(
   const PhysicalDeviceHandle physical_device,
   const VkDeviceQueueCreateFlags create_flags,
   const uint32_t queue_family_index,
   const uint32_t queue_count );

PhysicalDeviceHandle GetPhysicalDevice(
   const DeviceHandle & device );

} // namespace vkl

#endif // _VKL_DEVICE_H_
