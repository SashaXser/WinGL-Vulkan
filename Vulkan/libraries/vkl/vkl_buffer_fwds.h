#ifndef _VKL_BUFFER_FWDS_H_
#define _VKL_BUFFER_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using BufferHandle =
   std::shared_ptr< VkBuffer >;

} // namespace vkl

#endif // _VKL_BUFFER_FWDS_H_
