# Ember/engine/backends/vulkanRenderer/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(vulkanRendererBackend)

# Static vs Shared library option:
option(BUILD_SHARED_LIBS "Build ${PROJECT_NAME} as a shared library" ON) # default ON

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directors for add_executable(MyExe ...)     => .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/*:
file(GLOB SOURCE_FILES "src/*")
source_group("Source Files" FILES ${SOURCE_FILES})

# src/buffers/*:
file(GLOB BUFFERS_FILES "src/buffers/*")
source_group("Buffers" FILES ${BUFFERS_FILES})

# src/compute/*:
file(GLOB COMPUTE_FILES "src/compute/*")
source_group("Compute" FILES ${COMPUTE_FILES})

# src/context/*:
file(GLOB CONTEXT_FILES "src/context/*")
source_group("Context" FILES ${CONTEXT_FILES})

# src/gpuResourcePools/*:
file(GLOB GPU_RESOURCE_POOLS_FILES "src/gpuResourcePools/*")
source_group("Gpu Resource Pools" FILES ${GPU_RESOURCE_POOLS_FILES})

# src/gpuResources/*:
file(GLOB GPU_RESOURCES_FILES "src/gpuResources/*")
source_group("Gpu Resources" FILES ${GPU_RESOURCES_FILES})

# src/pipelines/*:
file(GLOB PIPELINES_FILES "src/pipelines/*")
source_group("Pipelines" FILES ${PIPELINES_FILES})

# src/pushConstants/*:
file(GLOB PUSH_CONSTANTS_FILES "src/pushConstants/*")
source_group("Push Constants" FILES ${PUSH_CONSTANTS_FILES})

# src/renderer/*:
file(GLOB RENDERER_FILES "src/renderer/*")
source_group("Renderer" FILES ${RENDERER_FILES})

# src/renderPasses/*:
file(GLOB RENDER_PASSES_FILES "src/renderPasses/*")
source_group("Render Passes" FILES ${RENDER_PASSES_FILES})

# src/textures/*:
file(GLOB TEXTURES_FILES "src/textures/*")
source_group("Textures" FILES ${TEXTURES_FILES})

# src/utility/*:
file(GLOB UTILITY_FILES "src/utility/*")
source_group("Utility" FILES ${UTILITY_FILES})

# ../../interfaces/math/*:
file(GLOB MATH_INTERFACE_FILES "../../interfaces/math/*")
source_group("Math Interface" FILES ${MATH_INTERFACE_FILES})

# ../../interfaces/renderer/*:
file(GLOB RENDERER_INTERFACE_FILES "../../interfaces/renderer/*")
source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

# ../../interfaces/window/*:
file(GLOB WINDOW_INTERFACE_FILES "../../interfaces/window/*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${BUFFERS_FILES}
    ${COMPUTE_FILES}
    ${CONTEXT_FILES}
    ${GPU_RESOURCE_POOLS_FILES}
    ${GPU_RESOURCES_FILES}
    ${PIPELINES_FILES}
    ${PUSH_CONSTANTS_FILES}
    ${RENDERER_FILES}
    ${RENDER_PASSES_FILES}
    ${TEXTURES_FILES}
    ${UTILITY_FILES}
    ${MATH_INTERFACE_FILES}
    ${RENDERER_INTERFACE_FILES}
    ${WINDOW_INTERFACE_FILES})

# Only expose public files to consumer:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compute
    ${CMAKE_CURRENT_SOURCE_DIR}/src/context
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResourcePools
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipelines
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pushConstants
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderPasses
    ${CMAKE_CURRENT_SOURCE_DIR}/src/textures
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utility
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/math
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VULKAN_RENDERER_BACKEND_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE VULKAN_RENDERER_BACKEND_STATIC)
endif()
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
# Vulkan (dynamicly linked):
find_package(Vulkan REQUIRED)           # required to be installed
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
    
# Vulkan Memory Allocator: (header only)
target_include_directories(${PROJECT_NAME} PUBLIC ../../libs/vma/include)

# SPIRV-Reflect:
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE) # enable static lib compilation for SPIRV-Reflect
add_subdirectory("../../libs/SPIRV-Reflect" "${CMAKE_BINARY_DIR}/SPIRV-Reflect_build")
#target_include_directories(${PROJECT_NAME} PUBLIC ../../libs/SPIRV-Reflect)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-reflect-static)
# ---------------------------------------------------