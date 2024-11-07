cmake_minimum_required(VERSION 3.10)

# Project name:
project(MyEngine)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# Source, header, shader files:
file(GLOB ALL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/*"
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsl"
    "${PROJECT_SOURCE_DIR}/shaders/*.glsl")
file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*")
file(GLOB VULKAN_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/vulkan*"
    "${PROJECT_SOURCE_DIR}/src/vma*"
    "${PROJECT_SOURCE_DIR}/src/sdl*"
    "${PROJECT_SOURCE_DIR}/src/pushConstant*"
    "${PROJECT_SOURCE_DIR}/src/material.*"
    "${PROJECT_SOURCE_DIR}/src/renderPass.h"
    "${PROJECT_SOURCE_DIR}/src/renderPass.cpp"
    "${PROJECT_SOURCE_DIR}/src/forwardRenderPass*"
    "${PROJECT_SOURCE_DIR}/src/shadowRenderPass*"
    "${PROJECT_SOURCE_DIR}/src/*Pipeline*")
file(GLOB MANAGER_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/logger*"
    "${PROJECT_SOURCE_DIR}/src/macros*"
    "${PROJECT_SOURCE_DIR}/src/time*"
    "${PROJECT_SOURCE_DIR}/src/*Manager*"
    "${PROJECT_SOURCE_DIR}/src/*Manager*")
file(GLOB SHADER_FILES
    #"${PROJECT_SOURCE_DIR}/shaders/*.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/spirvReflectTest.hlsl"
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsl"
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsli")
file(GLOB MATH_FILES "${PROJECT_SOURCE_DIR}/src/mathf.*")
file(GLOB COMPONENTS
    "${PROJECT_SOURCE_DIR}/src/component.*"
    "${PROJECT_SOURCE_DIR}/src/transform.*"
    "${PROJECT_SOURCE_DIR}/src/camera.*"
    "${PROJECT_SOURCE_DIR}/src/meshRenderer.*"
    "${PROJECT_SOURCE_DIR}/src/spin.*"
    "${PROJECT_SOURCE_DIR}/src/directionalLight.*"
    "${PROJECT_SOURCE_DIR}/src/spotLight.*")

# Remove Vulkan files from the default source/header lists:
list(REMOVE_ITEM SOURCE_FILES ${VULKAN_SOURCE_FILES} ${MANAGER_SOURCE_FILES} ${MATH_FILES} ${COMPONENTS})

# Groups for IDE display:
source_group("Components" FILES ${COMPONENTS})
source_group("Managers" FILES ${MANAGER_SOURCE_FILES})
source_group("Math" FILES ${MATH_FILES})
source_group("Shaders" FILES ${SHADER_FILES})
source_group("Source" FILES ${SOURCE_FILES})
source_group("Vulkan Renderer" FILES ${VULKAN_SOURCE_FILES})

# Mark shader files as header-only to prevent compilation
foreach(SHADER_FILE ${SHADER_FILES})
    set_source_files_properties(${SHADER_FILE} PROPERTIES HEADER_FILE_ONLY ON)
endforeach()


# Required libraries (location of CMakeLists.txt):
add_subdirectory(libs/SDL)              # SDL submodule
add_subdirectory(libs/spdlog)           # spdlog submodule
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE) # enable static lib compilation for SPIRV-Reflect
add_subdirectory(libs/SPIRV-Reflect)    # SPIRV-Reflect submodule
find_package(Vulkan REQUIRED)           # required to be installed

# Shader Language (pick one):
set(SHADER_LANGUAGE "HLSL") # HLSL or GLSL

# Compile shaders:
if(UNIX)
    add_custom_target(build_shaders ALL
        COMMAND "{PROJECT_SOURCE_DIR}/shaders/compile${SHADER_LANGUAGE}.sh"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
        BYPRODUCTS ${PROJECT_SOURCE_DIR}/shaders/*.spv)
endif(UNIX)

if(WIN32)
    add_custom_target(build_shaders ALL
        COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile${SHADER_LANGUAGE}.bat"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
        BYPRODUCTS ${PROJECT_SOURCE_DIR}/shaders/*.spv)
endif(WIN32)

# Configure MyEngine executable:
add_executable(MyEngine ${ALL_SOURCE_FILES})
add_dependencies(MyEngine build_shaders)

# Add headers (location of headers to include, e.g. SDL3/SDL.h in case of SDL):
target_include_directories(MyEngine PUBLIC libs/SDL/include)
target_include_directories(MyEngine PUBLIC libs/spdlog/include)
target_include_directories(MyEngine PUBLIC libs/SPIRV-Reflect)
target_include_directories(MyEngine PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(MyEngine PUBLIC libs/vma/include)        # header only library

# Add dynamic and static libraries (name of shared library in CMakeLists.txt of precombiled library):
target_link_libraries(MyEngine PUBLIC SDL3::SDL3-shared)        # SDL3
target_link_libraries(MyEngine PUBLIC spirv-reflect-static)     # SPRIV-Reflect
target_link_libraries(MyEngine PUBLIC ${Vulkan_LIBRARIES})      # Vulkan