#ifndef _VKL_MEMORY_FWDS_H_
#define _VKL_MEMORY_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using DeviceMemoryHandle =
   std::unique_ptr<
      VkDeviceMemory,
      void (*) ( const VkDeviceMemory * const ) >;

using MappedDeviceMemoryHandle =
   std::unique_ptr<
      void *,
      void (*) ( const void * const * const ) >;

} // namespace vkl

#endif // _VKL_MEMORY_FWDS_H_
