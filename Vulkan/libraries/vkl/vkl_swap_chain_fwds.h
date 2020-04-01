#ifndef _VKL_SWAP_CHAIN_FWDS_H_
#define _VKL_SWAP_CHAIN_FWDS_H_

#include <vulkan/vulkan.h>

#include <memory>

namespace vkl
{

using SwapChainHandle =
   std::shared_ptr< VkSwapchainKHR >;

} // namespace vkl

#endif // _VKL_SWAP_CHAIN_FWDS_H_
