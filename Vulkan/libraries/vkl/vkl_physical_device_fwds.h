#ifndef _VKL_PHYSICAL_DEVICE_FWDS_H_
#define _VKL_PHYSICAL_DEVICE_FWDS_H_

#include "vulkan/vulkan.h"

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

using PhysicalDeviceQueueFamilyProperties =
   std::vector<
      std::pair<
         uint32_t,
         VkQueueFamilyProperties > >;

} // namespace vkl

#endif // _VKL_PHYSICAL_DEVICE_FWDS_H_
