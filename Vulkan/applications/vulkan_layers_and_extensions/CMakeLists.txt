cmake_minimum_required(VERSION 3.15.0)

set(target_name vulkan-layers-and-extensions)

add_executable(${target_name} main.cpp)

find_package(Vulkan REQUIRED)
target_link_libraries(
   ${target_name}
   PRIVATE
   Vulkan::Vulkan)

set_target_properties(
   ${target_name}
   PROPERTIES
   FOLDER
   "${VULKAN_APPLICATION_IDE_FOLDER}")
