#include "vkl_command_buffer.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>

namespace vkl
{

struct Context
{
   VkPhysicalDevice physical_device;
   DeviceHandle device;
   CommandPoolHandle command_pool;
   VkCommandBufferLevel command_buffer_level;
};

void FreeCommandBuffer(
   const VkCommandBuffer * const command_buffer )
{
   if (command_buffer)
   {
      if (*command_buffer)
      {
         const auto * const context =
            vkl::internal::GetContextData<
               Context >(
                  command_buffer);

         if (context &&
             context->device && *context->device &&
             context->command_pool && *context->command_pool)
         {
            vkFreeCommandBuffers(
               *context->device,
               *context->command_pool,
               1,
               command_buffer);
         }
      }

      vkl::internal::DeallocateContext(
         command_buffer);
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

AllocatedCommandBuffers
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
         for (const auto & allocated_command_buffer :
              allocated_command_buffers)
         {
            const auto command_buffer =
               vkl::internal::AllocateContext<
                  VkCommandBuffer,
                  Context >(
                     GetPhysicalDevice(device),
                     device,
                     command_pool,
                     command_buffer_level);

            if (command_buffer)
            {
               command_buffers.emplace_back(
                  command_buffer,
                  &FreeCommandBuffer);

               *command_buffers.back() =
                  allocated_command_buffer;
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

DeviceHandle GetDevice(
   const CommandBufferHandle & command_buffer )
{
   return
      vkl::internal::GetContextData(
         command_buffer.get(),
         &Context::device);
}

VkPhysicalDevice GetPhysicalDevice(
   const CommandBufferHandle & command_buffer )
{
   return
      vkl::internal::GetContextData(
         command_buffer.get(),
         &Context::physical_device);
}

} // namespace vkl
