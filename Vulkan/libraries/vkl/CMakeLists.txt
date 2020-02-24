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
   vkl_physical_device_fwds.h)

find_package(Vulkan REQUIRED)
target_link_libraries(
   ${target_name}
   PUBLIC
   Vulkan::Vulkan)

target_include_directories(
   ${target_name}
   PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}/..")

set_target_properties(
   ${target_name}
   PROPERTIES
   FOLDER
   "${VULKAN_LIBRARIES_IDE_FOLDER}")