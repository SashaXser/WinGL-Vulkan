#ifndef _VKL_PHYSICAL_DEVICE_H_
#define _VKL_PHYSICAL_DEVICE_H_

#include "vkl_instance_fwds.h"
#include "vkl_physical_device_fwds.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>
#include <utility>

namespace vkl
{

PhysicalDevices GetPhysicalDevices(
   const InstanceHandle & instance );

PhysicalDevices GetPhysicalGPUDevices(
   const InstanceHandle & instnace );

PhysicalDeviceQueueFamilyProperties
GetPhysicalDeviceQueueFamilyProperties(
   const VkPhysicalDevice physical_device,
   const VkQueueFlags required,
   const VkQueueFlags preferred );

bool PhysicalDeviceSupportsPresentation(
   const VkPhysicalDevice physical_device,
   const uint32_t queue_family_index );

}  // namespace vkl

#endif // _VKL_PHYSICAL_DEVICE_H_
