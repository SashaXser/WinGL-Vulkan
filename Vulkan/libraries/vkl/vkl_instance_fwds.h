#ifndef _VKL_INSTANCE_FWDS_H_
#define _VKL_INSTANCE_FWDS_H_

#include "vulkan/vulkan.h"

#include <memory>

namespace vkl
{

using InstanceHandle =
   std::shared_ptr< VkInstance >;

} // namespace vkl

#endif // _VKL_INSTANCE_FWDS_H_
