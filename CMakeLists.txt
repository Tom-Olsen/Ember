# Engine/CMakeLists.txt

# Choose SHARED (.dll) or STATIC (.lib) library
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name
cmake_minimum_required(VERSION 3.31)
project(MyEngine)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# Source, header, shader files:
file(GLOB SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/*"
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsl"
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsli"
    "${PROJECT_SOURCE_DIR}/shaders/*.bat")

# Mathf files:
file(GLOB MATHF_FILES "${PROJECT_SOURCE_DIR}/src/mathf/*")
source_group("Mathf" FILES ${MATHF_FILES})

# VulkanContext files:
file(GLOB VULKAN_CONTEXT_FILES "${PROJECT_SOURCE_DIR}/src/vulkanContext/*")
source_group("VulkanContext" FILES ${VULKAN_CONTEXT_FILES})


# Configure executables:
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${MATHF_FILES} ${VULKAN_CONTEXT_FILES})
add_dependencies(${PROJECT_NAME} build_shaders)



# --------------------- Shaders ---------------------
set(SHADER_LANGUAGE "HLSL") # Set shader Language (HLSL or GLSL):

# Find shader files:
file(GLOB SHADER_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsl"
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsli")

# Mark shader files as header-only to prevent compilation
foreach(SHADER_FILE ${SHADER_FILES})
    set_source_files_properties(${SHADER_FILE} PROPERTIES HEADER_FILE_ONLY ON)
endforeach()

# Unix compilation script:
if(UNIX)
    add_custom_target(build_shaders ALL
        COMMAND "{PROJECT_SOURCE_DIR}/shaders/compile${SHADER_LANGUAGE}.sh"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
        BYPRODUCTS ${PROJECT_SOURCE_DIR}/shaders/*.spv)
endif(UNIX)

# Windows compilation script:
if(WIN32)
    add_custom_target(build_shaders ALL
        COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile${SHADER_LANGUAGE}.bat"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
        BYPRODUCTS ${PROJECT_SOURCE_DIR}/shaders/*.spv)
endif(WIN32)
# ---------------------------------------------------



# -------------------- Libraries --------------------
# SDL (cMakeList target):
add_subdirectory(libs/SDL)
target_include_directories(${PROJECT_NAME} PUBLIC libs/SDL/include)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3-shared)

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
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/mathf)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/vulkanContext)
# ---------------------------------------------------