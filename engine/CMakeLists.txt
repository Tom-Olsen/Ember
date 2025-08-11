# Ember/engine/CMakeLists.txt



# ------------------- Project Setup ------------------
# Choose SHARED (.dll) or STATIC (.lib) library (currently only static supported):
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(EmberEngine)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin") # target directory for add_executable(...)           => .exe
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll

# Compile options:
option(DEAR_IMGUI_ENABLED "Enable Dear ImGui" OFF)
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/engine.h:
file(GLOB ENGINE_FILE "${PROJECT_SOURCE_DIR}/src/emberEngine.h")
source_group("/" FILES ${ENGINE_FILE})

# src/buffers/*:
file(GLOB BUFFERS_FILES "${PROJECT_SOURCE_DIR}/src/buffers/*")
source_group("Buffers" FILES ${BUFFERS_FILES})

# src/components/*:
file(GLOB COMPONENTS_FILES "${PROJECT_SOURCE_DIR}/src/components/*")
source_group("Components" FILES ${COMPONENTS_FILES})

# src/compute/*:
file(GLOB COMPUTE_FILES "${PROJECT_SOURCE_DIR}/src/compute/*")
source_group("Compute" FILES ${COMPUTE_FILES})

# src/coreSystems/*:
file(GLOB CORE_SYSTEM_FILES "${PROJECT_SOURCE_DIR}/src/coreSystems/*")
source_group("Core Systems" FILES ${CORE_SYSTEM_FILES})

# src/dearImGui/*:
file(GLOB DEAR_IMGUI_FILES "${PROJECT_SOURCE_DIR}/src/dearImGui/*")
source_group("Dear ImGui" FILES ${DEAR_IMGUI_FILES})

# Imgui files:
if(DEAR_IMGUI_ENABLED)
    set(DEAR_IMGUI_FILES
        "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGui.h"
        "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGuiEnabled.h"
        "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGuiEnabled.cpp")
    set(IMGUI_FILES
        libs/imgui/imgui.cpp
        libs/imgui/imgui_demo.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_sdl3.cpp
        libs/imgui/backends/imgui_impl_vulkan.cpp)
    set(IMGUI_EXAMPLE libs/imgui/examples/example_sdl3_vulkan/main.cpp)
    source_group("ImGui Demo" FILES ${IMGUI_EXAMPLE})
else()
    set(DEAR_IMGUI_FILES
        "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGui.h"
        "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGuiDisabled.h")
    set(IMGUI_FILES "")
endif()

# src/editor/*:
file(GLOB EDITOR_FILES "${PROJECT_SOURCE_DIR}/src/editor/*")
source_group("Editor" FILES ${EDITOR_FILES})

# src/editorWindows/*:
file(GLOB EDITOR_WINDOWS_FILES "${PROJECT_SOURCE_DIR}/src/editorWindows/*")
source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})

# src/gameObjectSystem/*:
file(GLOB GAME_OBJECT_SYSTEM_FILES "${PROJECT_SOURCE_DIR}/src/gameObjectSystem/*")
source_group("Game Object System" FILES ${GAME_OBJECT_SYSTEM_FILES})

# src/graphics/*:
file(GLOB GRAPHICS_FILES "${PROJECT_SOURCE_DIR}/src/graphics/*")
source_group("Graphics" FILES ${GRAPHICS_FILES})

# src/managers/*:
file(GLOB MANAGERS_FILES "${PROJECT_SOURCE_DIR}/src/managers/*")
source_group("Managers" FILES ${MANAGERS_FILES})

# src/math/*:
file(GLOB MATH_FILES "${PROJECT_SOURCE_DIR}/src/math/*")
source_group("Math" FILES ${MATH_FILES})

# src/math/*:
file(GLOB PHYSICS_FILES "${PROJECT_SOURCE_DIR}/src/physics/*")
source_group("Physics" FILES ${PHYSICS_FILES})

# src/gpuResources/*:
file(GLOB GPU_RESOURCES_FILES "${PROJECT_SOURCE_DIR}/src/gpuResources/*")
source_group("GPU Resources" FILES ${GPU_RESOURCES_FILES})

# src/textures/*:
file(GLOB TEXTURES_FILES "${PROJECT_SOURCE_DIR}/src/textures/*")
source_group("Textures" FILES ${TEXTURES_FILES})

# src/utility/*:
file(GLOB UTILITY_FILES "${PROJECT_SOURCE_DIR}/src/utility/*")
source_group("Utility" FILES ${UTILITY_FILES})

# src/vulkanBackend/*:
file(GLOB VULKAN_BACKEND_FILES "${PROJECT_SOURCE_DIR}/src/vulkanBackend/*")
source_group("Vulkan Backend" FILES ${VULKAN_BACKEND_FILES})

# src/vulkanBackend/pipelines/*:
file(GLOB VULKAN_PIPELINES_FILES "${PROJECT_SOURCE_DIR}/src/vulkanBackend/pipelines/*")
source_group("Vulkan Backend\\Pipelines" FILES ${VULKAN_PIPELINES_FILES})

# src/vulkanBackend/pushConstants/*:
file(GLOB VULKAN_PUSH_CONSTANTS_FILES "${PROJECT_SOURCE_DIR}/src/vulkanBackend/pushConstants/*")
source_group("Vulkan Backend\\Push Constants" FILES ${VULKAN_PUSH_CONSTANTS_FILES})

# src/vulkanBackend/renderPasses/*:
file(GLOB VULKAN_RENDER_PASSES_FILES "${PROJECT_SOURCE_DIR}/src/vulkanBackend/renderPasses/*")
source_group("Vulkan Backend\\Render Passes" FILES ${VULKAN_RENDER_PASSES_FILES})

# src/vulkanBackend/utility/*:
file(GLOB VULKAN_UTILITY_FILES "${PROJECT_SOURCE_DIR}/src/vulkanBackend/utility/*")
file(GLOB CPP_HLSL_HEADERS "${PROJECT_SOURCE_DIR}/src/shaders/*.h")   # grab .h files form shaders, which contian constans for c++ and hlsl
list(APPEND VULKAN_UTILITY_FILES ${CPP_HLSL_HEADERS})
source_group("Vulkan Backend\\Utility" FILES ${VULKAN_UTILITY_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${PROJECT_NAME} STATIC
${ENGINE_FILE}
${BUFFERS_FILES}
${COMPONENTS_FILES}
${COMPUTE_FILES}
${EDITOR_FILES}
${EDITOR_WINDOWS_FILES}
${CORE_SYSTEM_FILES}
${DEAR_IMGUI_FILES}
${GAME_OBJECT_SYSTEM_FILES}
${GRAPHICS_FILES}
${IMGUI_FILES}
${IMGUI_EXAMPLE}   # add the example as a source to make it show up in the IDE, but mark it as header only, to prevent compilation (see below)
${MANAGERS_FILES}
${MATH_FILES}
${PHYSICS_FILES}
${GPU_RESOURCES_FILES}
${TEXTURES_FILES}
${UTILITY_FILES}
${VULKAN_BACKEND_FILES}
${VULKAN_PIPELINES_FILES}
${VULKAN_PUSH_CONSTANTS_FILES}
${VULKAN_RENDER_PASSES_FILES}
${VULKAN_UTILITY_FILES})
set_source_files_properties(${IMGUI_EXAMPLE} PROPERTIES HEADER_FILE_ONLY ON)    # don't compile these files

# Source subdirectories:
target_include_directories(${PROJECT_NAME}
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/buffers
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/components
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/compute
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/editorWindows
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/coreSystems
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/dearImGui
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gameObjectSystem
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/managers
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/math
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/physics
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders # needed for .h files form shaders, which contian constans for c++ and hlsl
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/textures
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/utility
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend/pipelines
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend/pushConstants
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend/renderPasses
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend/utility)

# Add project root path as a macro:
get_filename_component(ENGINE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_ROOT_PATH="${ENGINE_ROOT_PATH}")

# Add wether dear imgui is enabled as a macro:
if(DEAR_IMGUI_ENABLED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEAR_IMGUI_ENABLED=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEAR_IMGUI_ENABLED=0)
endif()

# Enable utf-8 encoding for microsoft visual studio: (TODO: fix files so i can remove this)
if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /utf-8)
endif()
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
# Shaders:
add_subdirectory("src/shaders")
add_dependencies(${PROJECT_NAME} CompileShaders)

# Imgui:
if(DEAR_IMGUI_ENABLED)
    target_include_directories(${PROJECT_NAME} PUBLIC libs/imgui libs/imgui/backends)
endif()

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

# Taskflow: (header only)
target_include_directories(${PROJECT_NAME} PUBLIC libs/taskflow)
# ---------------------------------------------------