#ifndef _VKL_BUFFER_VIEW_FWDS_H_
#define _VKL_BUFFER_VIEW_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using BufferViewHandle =
   std::shared_ptr< VkBufferView >;

} // namespace vkl

#endif // _VKL_BUFFER_VIEW_FWDS_H_
