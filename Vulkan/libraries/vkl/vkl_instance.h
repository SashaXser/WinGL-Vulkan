#ifndef _VKL_INSTANCE_H_
#define _VKL_INSTANCE_H_

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>

namespace vkl
{

using InstanceHandle =
   std::shared_ptr< VkInstance >;

InstanceHandle CreateInstance(
   const char * const application_name,
   const uint32_t application_version,
   const char * const engine_name,
   const uint32_t engine_version,
   const uint32_t vk_api_version_major,
   const uint32_t vk_api_version_minor,
   const uint32_t vk_api_version_patch );

} // namespace vkl

#endif // _VKL_INSTANCE_H_
