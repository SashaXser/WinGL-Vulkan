#ifndef _VKL_INSTANCE_FWDS_H_
#define _VKL_INSTANCE_FWDS_H_

#include "vulkan/vulkan.h"

#include <memory>

namespace vkl
{

using InstanceHandle =
   std::unique_ptr<
      VkInstance,
      void (*) ( const VkInstance * const ) >;

} // namespace vkl

#endif // _VKL_INSTANCE_FWDS_H_
