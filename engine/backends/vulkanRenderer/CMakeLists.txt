# Ember/engine/backends/vulkanRenderer/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(vulkanRendererBackend)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed to link static libraries into this dynamic library.

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
# *.* makes sure to only include files and not folders.
# src/compute/*.*:
file(GLOB COMPUTE_FILES "src/compute/*.*")
source_group("Compute" FILES ${COMPUTE_FILES})

# src/context/*.*:
file(GLOB CONTEXT_FILES "src/context/*.*")
source_group("Context" FILES ${CONTEXT_FILES})

# src/gpuResourcePools/*.*:
file(GLOB GPU_RESOURCE_POOLS_FILES "src/gpuResourcePools/*.*")
source_group("Gpu Resource Pools" FILES ${GPU_RESOURCE_POOLS_FILES})

# src/gpuResources/*.*:
file(GLOB GPU_RESOURCES_FILES "src/gpuResources/*.*")
source_group("Gpu Resources" FILES ${GPU_RESOURCES_FILES})

# src/gpuResources/buffers/*.*:
file(GLOB BUFFERS_FILES "src/gpuResources/buffers/*.*")
source_group("Gpu Resources/Buffers" FILES ${BUFFERS_FILES})

# src/gpuResources/samplers/*.*:
file(GLOB SAMPLERS_FILES "src/gpuResources/samplers/*.*")
source_group("Gpu Resources/Samplers" FILES ${SAMPLERS_FILES})

# src/gpuResources/textures/*.*:
file(GLOB TEXTURES_FILES "src/gpuResources/textures/*.*")
source_group("Gpu Resources/Textures" FILES ${TEXTURES_FILES})

# src/gpuResources/textures/texture1d/*.*:
file(GLOB TEXTURE_1D_FILES "src/gpuResources/textures/texture1d/*.*")
source_group("Gpu Resources/Textures/Texture1d" FILES ${TEXTURE_1D_FILES})

# src/gpuResources/textures/texture2d/*.*:
file(GLOB TEXTURE_2D_FILES "src/gpuResources/textures/texture2d/*.*")
source_group("Gpu Resources/Textures/Texture2d" FILES ${TEXTURE_2D_FILES})

# src/gpuResources/textures/texture3d/*.*:
file(GLOB TEXTURE_3D_FILES "src/gpuResources/textures/texture3d/*.*")
source_group("Gpu Resources/Textures/Texture3d" FILES ${TEXTURE_3D_FILES})

# src/gpuResources/textures/textureCube/*.*:
file(GLOB TEXTURE_CUBE_FILES "src/gpuResources/textures/textureCube/*.*")
source_group("Gpu Resources/Textures/TextureCube" FILES ${TEXTURE_CUBE_FILES})

# src/pipelines/*.*:
file(GLOB PIPELINES_FILES "src/pipelines/*.*")
source_group("Pipelines" FILES ${PIPELINES_FILES})

# src/pushConstants/*.*:
file(GLOB PUSH_CONSTANTS_FILES "src/pushConstants/*.*")
source_group("Push Constants" FILES ${PUSH_CONSTANTS_FILES})

# src/renderer/*.*:
file(GLOB RENDERER_FILES "src/renderer/*.*")
source_group("Renderer" FILES ${RENDERER_FILES})

# src/renderPasses/*.*:
file(GLOB RENDER_PASSES_FILES "src/renderPasses/*.*")
source_group("Render Passes" FILES ${RENDER_PASSES_FILES})

# src/types/*.*:
file(GLOB TYPES_FILES "src/types/*.*")
source_group("Types" FILES ${TYPES_FILES})

# src/utility/*.*:
file(GLOB UTILITY_FILES "src/utility/*.*")
source_group("Utility" FILES ${UTILITY_FILES})

# ../../common/*.*:
file(GLOB COMMON_INTERFACE_FILES "../../common/*.*")
source_group("Commen" FILES ${COMMON_INTERFACE_FILES})

# ../../interfaces/renderer/*.*:
file(GLOB RENDERER_INTERFACE_FILES "../../interfaces/renderer/*.*")
source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

# ../../interfaces/window/*.*:
file(GLOB WINDOW_INTERFACE_FILES "../../interfaces/window/*.*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${PROJECT_NAME}
    ${COMPUTE_FILES}
    ${CONTEXT_FILES}
    ${GPU_RESOURCE_POOLS_FILES}
    ${GPU_RESOURCES_FILES}
        ${BUFFERS_FILES}
        ${SAMPLERS_FILES}
        ${TEXTURES_FILES}
            ${TEXTURE_1D_FILES}
            ${TEXTURE_2D_FILES}
            ${TEXTURE_3D_FILES}
            ${TEXTURE_CUBE_FILES}
    ${PIPELINES_FILES}
    ${PUSH_CONSTANTS_FILES}
    ${RENDERER_FILES}
    ${RENDER_PASSES_FILES}
    ${TYPES_FILES}
    ${UTILITY_FILES}
    ${COMMON_INTERFACE_FILES}
    ${RENDERER_INTERFACE_FILES}
    ${WINDOW_INTERFACE_FILES})

# Only expose public files to consumer:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compute
    ${CMAKE_CURRENT_SOURCE_DIR}/src/context
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResourcePools
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources/buffers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources/samplers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources/textures
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources/textures/texture1d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources/textures/texture2d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources/textures/texture3d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources/textures/textureCube
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipelines
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pushConstants
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderPasses
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utility
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
# Path to vulkan library source directory:
get_filename_component(VULKAN_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_LIBRARY_ROOT_PATH="${VULKAN_LIBRARY_ROOT_PATH}")

# Library compilation logic:
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VULKAN_RENDERER_BACKEND_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE VULKAN_RENDERER_BACKEND_STATIC)
endif()
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberAssetLoader:
if (NOT TARGET EmberAssetLoader)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../libs/assetLoader" EmberAssetLoader)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberAssetLoader)

# EmberLogger:
if (NOT TARGET EmberLogger)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../libs/logger" EmberLogger)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberLogger)

# EmberMath:
if (NOT TARGET EmberMath)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../libs/math" EmberMath)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberMath)

# EmberProfiler:
if (NOT TARGET EmberProfiler)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../libs/profiler" EmberProfiler)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberProfiler)

# EmberTaskSystem:
if (NOT TARGET EmberTaskSystem)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../libs/taskSystem" EmberTaskSystem)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberTaskSystem)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# Vulkan (dynamicly linked):
find_package(Vulkan REQUIRED)           # required to be installed
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
    
# Vulkan Memory Allocator: (header only)
target_include_directories(${PROJECT_NAME} PUBLIC ../../extern/vma/include)

# SPIRV-Reflect:
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE) # enable static lib compilation for SPIRV-Reflect
add_subdirectory("../../extern/SPIRV-Reflect" "${CMAKE_BINARY_DIR}/SPIRV-Reflect_build")
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-reflect-static)
# ---------------------------------------------------



# ----------------- Compile Shaders -----------------
if(WIN32)
    set(SHADER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/compileShaders.bat")
    add_custom_target(CompileShaders ALL
        COMMAND "${SHADER_SCRIPT}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders"
        COMMENT "Compiling HLSL compute shaders (Windows)"
    )
elseif(UNIX)
    set(SHADER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/compileShaders.sh")
    add_custom_target(CompileShaders ALL
        COMMAND bash "${SHADER_SCRIPT}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders"
        COMMENT "Compiling HLSL compute shaders (Linux/Ubuntu)"
    )
endif()

# Make sure your Vulkan library depends on shaders being compiled first
add_dependencies(${PROJECT_NAME} CompileShaders)
# ---------------------------------------------------