#include "vkl_command_buffer.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>
#include <new>

namespace vkl
{

constexpr size_t DEVICE_INDEX = 1;
constexpr size_t PHYSICAL_DEVICE_INDEX = 2;
constexpr size_t COMMAND_POOL_INDEX = 3;
constexpr size_t COMMAND_BUFFER_LEVEL_INDEX = 4;
constexpr size_t CONTEXT_SIZE = 5;

void FreeCommandBuffer(
   const VkCommandBuffer * const command_buffer )
{
   if (command_buffer)
   {
      if (*command_buffer)
      {
         vkFreeCommandBuffers(
            vkl::internal::GetContextData<
               VkDevice,
               DEVICE_INDEX >(
                  command_buffer),
            vkl::internal::GetContextData<
               VkCommandPool,
               COMMAND_POOL_INDEX >(
                  command_buffer),
            1,
            command_buffer);
      }

      delete []
         (reinterpret_cast< const size_t * >(
            command_buffer) - (CONTEXT_SIZE - 1));
   }
}

CommandBufferHandle AllocateCommandBuffer(
   const DeviceHandle & device,
   const CommandPoolHandle & command_pool,
   const VkCommandBufferLevel command_buffer_level )
{
   auto command_buffers =
      AllocateCommandBuffers(
         device,
         command_pool,
         command_buffer_level,
         1);

   CommandBufferHandle command_buffer {
      nullptr, &FreeCommandBuffer };

   if (!command_buffers.empty())
   {
      command_buffer.swap(
         command_buffers.front());
   }

   return command_buffer;
}

std::vector< CommandBufferHandle >
AllocateCommandBuffers(
   const DeviceHandle & device,
   const CommandPoolHandle & command_pool,
   const VkCommandBufferLevel command_buffer_level,
   const uint32_t command_buffer_count )
{
   std::vector< CommandBufferHandle > command_buffers;

   if (device && *device &&
       command_pool && *command_pool)
   {
      std::vector< VkCommandBuffer > allocated_command_buffers {
         command_buffer_count, nullptr };

      const VkCommandBufferAllocateInfo info {
         VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
         nullptr,
         *command_pool,
         command_buffer_level,
         command_buffer_count
      };

      const auto result =
         vkAllocateCommandBuffers(
            *device,
            &info,
            allocated_command_buffers.data());

      if (result != VK_SUCCESS)
      {
         std::cerr
            << "Could not allocate "
            << command_buffer_count
            << "command buffers ("
            << result
            << ")!"
            << std::endl;
      }
      else
      {
         for (const auto & allocated_command_buffer : allocated_command_buffers)
         {
            size_t * const context =
               new (std::nothrow) size_t[CONTEXT_SIZE] { };

            if (context)
            {
               *(context + (CONTEXT_SIZE - PHYSICAL_DEVICE_INDEX - 1)) =
                  reinterpret_cast< size_t >(
                     GetPhysicalDevice(device));

               *(context + (CONTEXT_SIZE - DEVICE_INDEX - 1)) =
                  reinterpret_cast< size_t >(
                     *device);

               *(context + (CONTEXT_SIZE - COMMAND_POOL_INDEX - 1)) =
                  reinterpret_cast< size_t >(
                     *command_pool);

               *(context + (CONTEXT_SIZE - COMMAND_BUFFER_LEVEL_INDEX - 1)) =
                  command_buffer_level;

               command_buffers.emplace_back(
                  reinterpret_cast< VkCommandBuffer * >(
                     context + (CONTEXT_SIZE - 1)),
                  &FreeCommandBuffer);

               *command_buffers.back() = allocated_command_buffer;
            }
            else
            {
               command_buffers.clear();

               break;
            }
         }
      }
   }

   if (!command_buffers.empty())
   {
      std::cout
         << command_buffer_count
         << " command buffers allocated successfully!"
         << std::endl;
   }

   return command_buffers;
}

bool BeginCommandBuffer(
   const CommandBufferHandle & command_buffer,
   const VkCommandBufferUsageFlags command_buffer_usage_flags )
{
   const VkCommandBufferBeginInfo info {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      nullptr,
      command_buffer_usage_flags,
      nullptr
   };

   const auto result =
      vkBeginCommandBuffer(
         *command_buffer,
         &info);

   if (result != VK_SUCCESS)
   {
      std::cerr
         << "Starting command buffer failed ("
         << result
         << ")!"
         << std::endl;
   }

   return
      result == VK_SUCCESS;
}

bool EndCommandBuffer(
   const CommandBufferHandle & command_buffer )
{
   const auto result =
      vkEndCommandBuffer(
         *command_buffer);

   if (result != VK_SUCCESS)
   {
      std::cerr
         << "Finishing command buffer failed ("
         << result
         << ")!"
         << std::endl;
   }

   return
      result == VK_SUCCESS;
}

VkDevice GetDevice(
   const CommandBufferHandle & command_buffer )
{
   return
      vkl::internal::GetContextData<
         VkDevice,
         DEVICE_INDEX >(
            command_buffer.get());
}

VkPhysicalDevice GetPhysicalDevice(
   const CommandBufferHandle & command_buffer )
{
   return
      vkl::internal::GetContextData<
         VkPhysicalDevice,
         PHYSICAL_DEVICE_INDEX >(
            command_buffer.get());
}

} // namespace vkl
