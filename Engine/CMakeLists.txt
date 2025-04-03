# Ember/Engine/CMakeLists.txt

# Choose SHARED (.dll) or STATIC (.lib) library (currently only static supported):
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(EmberEngine)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")


   
# ------------------- File lists --------------------
# src/components/*:
file(GLOB COMPONENTS_FILES "${PROJECT_SOURCE_DIR}/src/components/*")
source_group("Components" FILES ${COMPONENTS_FILES})

# src/editor/*:
file(GLOB EDITOR_FILES "${PROJECT_SOURCE_DIR}/src/editor/*")
source_group("Editor" FILES ${EDITOR_FILES})

# src/engine/*:
file(GLOB ENGINE_FILES "${PROJECT_SOURCE_DIR}/src/engine/*")
source_group("Engine" FILES ${ENGINE_FILES})

# src/gameObjectSystem/*:
file(GLOB GAME_OBJECT_SYSTEM_FILES "${PROJECT_SOURCE_DIR}/src/gameObjectSystem/*")
source_group("Game Object System" FILES ${GAME_OBJECT_SYSTEM_FILES})

# Imgui files:
set(IMGUI_FILES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/backends/imgui_impl_sdl3.cpp
    libs/imgui/backends/imgui_impl_vulkan.cpp)
source_group("ImGui" FILES ${IMGUI_FILES})
set(IMGUI_EXAMPLE libs/imgui/examples/example_sdl3_vulkan/main.cpp)
source_group("ImGui Demo" FILES ${IMGUI_EXAMPLE})

# src/managers/*:
file(GLOB MANAGERS_FILES "${PROJECT_SOURCE_DIR}/src/managers/*")
source_group("Managers" FILES ${MANAGERS_FILES})

# src/math/*:
file(GLOB MATH_FILES "${PROJECT_SOURCE_DIR}/src/math/*")
source_group("Math" FILES ${MATH_FILES})

# src/pushConstants/*:
file(GLOB PUSH_CONSTANTS_FILES "${PROJECT_SOURCE_DIR}/src/pushConstants/*")
source_group("Push Constants" FILES ${PUSH_CONSTANTS_FILES})

# src/renderPassesAndPipelines/*:
file(GLOB RENDER_PASSES_AND_PIPELINES_FILES "${PROJECT_SOURCE_DIR}/src/renderPassesAndPipelines/*")
source_group("Render Passes & Pipelines" FILES ${RENDER_PASSES_AND_PIPELINES_FILES})

# src/renderResources/*:
file(GLOB RENDER_RESOURCES_FILES "${PROJECT_SOURCE_DIR}/src/renderResources/*")
source_group("Render Resources" FILES ${RENDER_RESOURCES_FILES})

# src/texture types/*:
file(GLOB TEXTURE_TYPES_FILES "${PROJECT_SOURCE_DIR}/src/textureTypes/*")
source_group("Texture Types" FILES ${TEXTURE_TYPES_FILES})

# src/utility/*:
file(GLOB UTILITY_FILES "${PROJECT_SOURCE_DIR}/src/utility/*")
source_group("Utility" FILES ${UTILITY_FILES})

# src/vulkanRenderer/*:
file(GLOB VULKAN_RENDERER_FILES "${PROJECT_SOURCE_DIR}/src/vulkanRenderer/*")
source_group("VulkanRenderer" FILES ${VULKAN_RENDERER_FILES})
# ---------------------------------------------------



# Configure library:
add_library(${PROJECT_NAME} STATIC
${COMPONENTS_FILES}
${EDITOR_FILES}
${ENGINE_FILES}
${GAME_OBJECT_SYSTEM_FILES}
${IMGUI_FILES}
${IMGUI_EXAMPLE}   # add the example as a source to make it show up in the IDE, but mark it as header only, to prevent compilation (see below)
${MANAGERS_FILES}
${MATH_FILES}
${PUSH_CONSTANTS_FILES}
${RENDER_PASSES_AND_PIPELINES_FILES}
${RENDER_RESOURCES_FILES}
${TEXTURE_TYPES_FILES}
${UTILITY_FILES}
${VULKAN_RENDERER_FILES})
set_source_files_properties(${IMGUI_EXAMPLE} PROPERTIES HEADER_FILE_ONLY ON)    # don't compile these files

# Add engine root path as a compile definition
get_filename_component(ENGINE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_ROOT_PATH="${ENGINE_ROOT_PATH}")

# Enable utf-8 encoding for microsoft visual studio: (TODO: fix files so i can remove this)
if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /utf-8)
endif()



# -------------------- Libraries --------------------
# Shaders:
add_subdirectory("src/shaders")
add_dependencies(${PROJECT_NAME} CompileShaders)

# Imgui:
target_include_directories(${PROJECT_NAME} PUBLIC libs/imgui libs/imgui/backends)

# SDL (cMakeList target):
set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)
add_subdirectory(libs/SDL)
target_include_directories(${PROJECT_NAME} PUBLIC libs/SDL/include)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3-static)
if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
endif(UNIX)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm dxguid)
endif(WIN32)

# spdlog (cMakeList target):
add_subdirectory(libs/spdlog)
target_include_directories(${PROJECT_NAME} PUBLIC libs/spdlog/include)

# SPIRV-Reflect (cMakeList target):
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE) # enable static lib compilation for SPIRV-Reflect
add_subdirectory(libs/SPIRV-Reflect)
target_include_directories(${PROJECT_NAME} PUBLIC libs/SPIRV-Reflect)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-reflect-static)

# Vulkan (dynamicly linked):
find_package(Vulkan REQUIRED)           # required to be installed
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})

# Vulkan Memory Allocator: (header only)
target_include_directories(${PROJECT_NAME} PUBLIC libs/vma/include)

# Source subdirectories:
target_include_directories(${PROJECT_NAME}
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/components
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gameobjectSystem
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/managers
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/math
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pushConstants
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/renderPassesAndPipelines
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/renderResources
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/textureTypes
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/utility
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanRenderer
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
# ---------------------------------------------------