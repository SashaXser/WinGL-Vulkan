cmake_minimum_required(VERSION 3.15.0)

set(target_name vkl)

add_library(
   ${target_name} STATIC
   vkl_allocator.cpp
   vkl_allocator.h
   vkl_buffer.cpp
   vkl_buffer.h
   vkl_buffer_fwds.h
   vkl_buffer_view.cpp
   vkl_buffer_view.h
   vkl_buffer_view_fwds.h
   vkl_command_buffer.cpp
   vkl_command_buffer.h
   vkl_command_buffer_fwds.h
   vkl_command_pool.cpp
   vkl_command_pool.h
   vkl_command_pool_fwds.h
   vkl_context_data.h
   vkl_device.cpp
   vkl_device.h
   vkl_device_fwds.h
   vkl_image.cpp
   vkl_image.h
   vkl_image_fwds.h
   vkl_image_view.cpp
   vkl_image_view.h
   vkl_image_view_fwds.h
   vkl_instance.cpp
   vkl_instance.h
   vkl_instance_fwds.h
   vkl_memory.cpp
   vkl_memory.h
   vkl_memory_fwds.h
   vkl_physical_device.cpp
   vkl_physical_device.h
   vkl_physical_device_fwds.h
   vkl_window.cpp
   vkl_window.h
   vkl_window_fwds.h
   vkl_window_input_action.h
   vkl_window_keyboard_keys.h
   vkl_window_modifier_keys.h
   vkl_window_mouse_buttons.h)

find_package(Vulkan REQUIRED)
target_link_libraries(
   ${target_name}
   PUBLIC
   Vulkan::Vulkan)

target_link_libraries(
   ${target_name}
   PRIVATE
   glfw)

target_include_directories(
   ${target_name}
   PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}/..")

if (WIN32)
   target_compile_definitions(
      ${target_name}
      PUBLIC
      VK_USE_PLATFORM_WIN32_KHR)
else ( )
   message(
      FATAL_ERROR
      "Define VK Platform Type!")
endif ( )

if (VKL_DISABLE_DEFAULT_VULKAN_ALLOCATOR)
   target_compile_definitions(
      ${target_name}
      PRIVATE
      VKL_DISABLE_DEFAULT_VULKAN_ALLOCATOR)
endif ( )

set_target_properties(
   ${target_name}
   PROPERTIES
   FOLDER
   "${VULKAN_LIBRARIES_IDE_FOLDER}")
