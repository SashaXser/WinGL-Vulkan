cmake_minimum_required(VERSION 3.15.0)

set(VULKAN_EXT_SRC_IDE_FOLDER
   "${VULKAN_IDE_FOLDER}/ext-src")

set(GLM_TEST_ENABLE off)
set(BUILD_SHARED_LIBS off)

add_subdirectory(glm-0.9.9.6)

set_target_properties(
   glm_static
   PROPERTIES
   FOLDER
   "${VULKAN_EXT_SRC_IDE_FOLDER}/glm")

set(BUILD_SHARED_LIBS off)
set(GLFW_BUILD_EXAMPLES off)
set(GLFW_BUILD_TESTS off)
set(GLFW_BUILD_DOCS off)
set(GLFW_INSTALL off)

add_subdirectory(glfw-3.3.2)

set_target_properties(
   glfw
   PROPERTIES
   FOLDER
   "${VULKAN_EXT_SRC_IDE_FOLDER}/glfw3")
