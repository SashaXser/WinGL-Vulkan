#include "vkl_command_pool.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>

namespace vkl
{

namespace
{

struct Context final
{
   PhysicalDeviceHandle physical_device;
   DeviceHandle device;
   VkCommandPoolCreateFlags create_flags;
   uint32_t queue_family_index;
};

} // namespace

void DestroyCommandPoolHandle(
   const VkCommandPool * const command_pool )
{
   if (command_pool)
   {
      if (*command_pool)
      {
         const auto device =
            vkl::internal::GetContextData(
               command_pool,
               &Context::device);

         if (device && *device)
         {
            vkDestroyCommandPool(
               *device,
               *command_pool,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         command_pool);
   }
}

CommandPoolHandle CreateCommandPool(
   const DeviceHandle & device,
   const VkCommandPoolCreateFlags create_flags,
   const uint32_t queue_family_index )
{
   CommandPoolHandle command_pool {
      nullptr, &DestroyCommandPoolHandle };

   if (device && *device)
   {
      command_pool.reset(
         vkl::internal::AllocateContext<
            VkCommandPool,
            Context >(
               GetPhysicalDevice(device),
               device,
               create_flags,
               queue_family_index),
         &DestroyCommandPoolHandle);

      if (command_pool)
      {
         const VkCommandPoolCreateInfo info {
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            nullptr,
            create_flags,
            queue_family_index
         };

         const auto success =
            vkCreateCommandPool(
               *device,
               &info,
               DefaultAllocator(),
               command_pool.get());

         if (success != VK_SUCCESS)
         {
            std::cerr
               << "Command pool not created successfully ("
               << success
               << ")!"
               << std::endl;

            command_pool.reset();
         }
         else
         {
            std::cout
               << "Command pool created successfully!"
               << std::endl;
         }
      }
   }

   return command_pool;
}

bool ResetCommandPool(
   const CommandPoolHandle & command_pool,
   const VkCommandPoolResetFlags & reset_flags )
{
   bool reset = false;

   if (command_pool && *command_pool)
   {
      if (const auto device =
          GetDevice(command_pool))
      {
         const auto result =
            vkResetCommandPool(
               *device,
               *command_pool,
               reset_flags);

         reset = result == VK_SUCCESS;

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Could not reset command pool ("
               << result
               << ")!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Command pool reset!"
               << std::endl;
         }
      }
   }

   return reset;
}

DeviceHandle GetDevice(
   const CommandPoolHandle & command_pool )
{
   return
      vkl::internal::GetContextData(
         command_pool.get(),
         &Context::device);
}

PhysicalDeviceHandle GetPhysicalDevice(
   const CommandPoolHandle & command_pool )
{
   return
      vkl::internal::GetContextData(
         command_pool.get(),
         &Context::physical_device);
}

} // namespace vkl
