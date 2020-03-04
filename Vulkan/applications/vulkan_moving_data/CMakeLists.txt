cmake_minimum_required(VERSION 3.15.0)

set(target_name vulkan-moving-data)

add_executable(${target_name} main.cpp)

target_link_libraries(
   ${target_name}
   PRIVATE
   vkl)

set_target_properties(
   ${target_name}
   PROPERTIES
   FOLDER
   "${VULKAN_APPLICATION_IDE_FOLDER}")
