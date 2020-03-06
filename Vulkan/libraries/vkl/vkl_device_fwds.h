#ifndef _VKL_DEVICE_FWDS_H_
#define _VKL_DEVICE_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using DeviceHandle =
   std::shared_ptr< VkDevice >;

} // namespace vkl

#endif // _VKL_DEVICE_FWDS_H_
