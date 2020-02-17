#ifndef _VKL_DEVICE_FWDS_H_
#define _VKL_DEVICE_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using DeviceHandle =
   std::unique_ptr<
      VkDevice,
      void (*) ( const VkDevice * const ) >;

} // namespace vkl

#endif // _VKL_DEVICE_FWDS_H_
