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
