#ifndef _VKL_SWAP_CHAIN_H_
#define _VKL_SWAP_CHAIN_H_

#include "vkl_device_fwds.h"
#include "vkl_image_fwds.h"
#include "vkl_physical_device_fwds.h"
#include "vkl_surface_fwds.h"
#include "vkl_swap_chain_fwds.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace vkl
{

SwapChainHandle CreateSwapChain(
   const DeviceHandle & device,
   const SurfaceHandle & surface );

SwapChainHandle CreateSwapChain(
   const SwapChainHandle & current_swap_chain );

DeviceHandle GetDevice(
   const SwapChainHandle & swap_chain );

PhysicalDeviceHandle GetPhysicalDevice(
   const SwapChainHandle & swap_chain );

SurfaceHandle GetSurface(
   const SwapChainHandle & swap_chain );

std::optional< VkFormat >
GetFormat(
   const SwapChainHandle & swap_chain );

std::optional< VkExtent2D >
GetExtent(
   const SwapChainHandle & swap_chain );

std::optional< std::vector< ImageHandle > >
GetSwapChainImages(
   const SwapChainHandle & swap_chain );

} // namespace vkl

#endif // _VKL_SWAP_CHAIN_H_
