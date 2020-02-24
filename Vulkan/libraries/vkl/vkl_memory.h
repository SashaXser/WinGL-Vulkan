#ifndef _VKL_MEMORY_H_
#define _VKL_MEMORY_H_

#include "vkl_device_fwds.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>

namespace vkl
{

using DeviceMemoryHandle =
   std::unique_ptr<
      VkDeviceMemory,
      void (*) ( const VkDeviceMemory * const ) >;

DeviceMemoryHandle AllocateDeviceMemory(
   const DeviceHandle & device,
   const VkDeviceSize size,
   const uint32_t type_index );

VkDevice GetDevice(
   const DeviceMemoryHandle & memory );

VkPhysicalDevice GetPhysicalDevice(
   const DeviceMemoryHandle & memory );

VkDeviceSize GetSize(
   const DeviceMemoryHandle & memory );

uint32_t GetTypeIndex(
   const DeviceMemoryHandle & memory );

using MappedDeviceMemoryHandle =
   std::unique_ptr<
      void *,
      void (*) ( const void * const * const ) >;

MappedDeviceMemoryHandle MapDeviceMemory(
   const DeviceHandle & device,
   const DeviceMemoryHandle & device_memory,
   const VkDeviceSize offset,
   const VkDeviceSize size,
   const VkMemoryMapFlags flags );

VkDevice GetDevice(
   const MappedDeviceMemoryHandle & mapped_memory );

VkPhysicalDevice GetPhysicalDevice(
   const MappedDeviceMemoryHandle & mapped_memory );

VkDeviceMemory GetDeviceMemory(
   const MappedDeviceMemoryHandle & mapped_memory );

VkDeviceSize GetOffset(
   const MappedDeviceMemoryHandle & mapped_memory );

VkDeviceSize GetSize(
   const MappedDeviceMemoryHandle & mapped_memory );

VkMemoryMapFlags GetMemoryMapFlags(
   const MappedDeviceMemoryHandle & mapped_memory );

} // namespace vkl

#endif // _VKL_MEMORY_H_