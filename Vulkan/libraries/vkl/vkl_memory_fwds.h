#ifndef _VKL_MEMORY_FWDS_H_
#define _VKL_MEMORY_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using DeviceMemoryHandle =
   std::shared_ptr< VkDeviceMemory >;

using MappedDeviceMemoryHandle =
   std::shared_ptr< void * >;

} // namespace vkl

#endif // _VKL_MEMORY_FWDS_H_
