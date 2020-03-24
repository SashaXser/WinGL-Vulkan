#ifndef _VKL_SURFACE_FWDS_H_
#define _VKL_SURFACE_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using SurfaceHandle =
   std::shared_ptr< VkSurfaceKHR >;

} // namespace vkl

#endif // _VKL_SURFACE_FWDS_H_
