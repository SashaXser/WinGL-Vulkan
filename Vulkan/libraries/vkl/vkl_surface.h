#ifndef _VKL_SURFACE_H_
#define _VKL_SURFACE_H_

#include "vkl_instance_fwds.h"
#include "vkl_physical_device_fwds.h"
#include "vkl_surface_fwds.h"
#include "vkl_window_fwds.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace vkl
{

SurfaceHandle CreateSurface(
   const PhysicalDeviceHandle & physical_device,
   const WindowHandle & window );

InstanceHandle GetInstance(
   const SurfaceHandle & surface );

PhysicalDeviceHandle GetPhysicalDevice(
   const SurfaceHandle & surface );

WindowHandle GetWindow(
   const SurfaceHandle & surface );

std::optional< VkSurfaceCapabilitiesKHR >
GetSurfaceCapabilites(
   const SurfaceHandle & surface );

std::optional< std::vector< VkSurfaceFormatKHR > >
GetSurfaceFormats(
   const SurfaceHandle & surface );

std::optional< std::vector< VkPresentModeKHR > >
GetSurfacePresentModes(
   const SurfaceHandle & surface );

} // namespace vkl

#endif // _VKL_SURFACE_H_
