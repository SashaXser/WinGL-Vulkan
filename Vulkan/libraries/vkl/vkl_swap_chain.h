#ifndef _VKL_SWAP_CHAIN_H_
#define _VKL_SWAP_CHAIN_H_

#include "vkl_device_fwds.h"
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

// todo, need to change to be handles
std::optional< std::vector< VkImage > >
GetSwapChainImages(
   const SwapChainHandle & swap_chain );

} // namespace vkl

#endif // _VKL_SWAP_CHAIN_H_
