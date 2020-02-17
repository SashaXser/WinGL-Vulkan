#ifndef _VKL_PHYSICAL_DEVICE_H_
#define _VKL_PHYSICAL_DEVICE_H_

#include "vkl_instance_fwds.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>
#include <utility>

namespace vkl
{

using PhysicalDevices =
   std::vector<
      std::pair<
         VkPhysicalDeviceProperties,
         VkPhysicalDevice > >;

PhysicalDevices GetPhysicalDevices(
   const InstanceHandle & instance );

PhysicalDevices GetPhysicalGPUDevices(
   const InstanceHandle & instnace );

using PhysicalDeviceQueueFamilyProperties =
   std::vector<
      std::pair<
         uint32_t,
         VkQueueFamilyProperties > >;

PhysicalDeviceQueueFamilyProperties
GetPhysicalDeviceQueueFamilyProperties(
   const VkPhysicalDevice physical_device,
   const VkQueueFlags required,
   const VkQueueFlags preferred );

}  // namespace vkl

#endif // _VKL_PHYSICAL_DEVICE_H_
