# Ember/engine/backends/vulkanRenderer/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(VulkanRendererBackend)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed to link static libraries into this dynamic library.

# Static vs Shared library option:
option(VULKAN_RENDERER_BACKEND_BUILD_SHARED "Build ${PROJECT_NAME} as a shared library" ON) # default = dynamic

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}/../../common" ABSOLUTE)            # common 
get_filename_component(INTERFACES_DIR "${CMAKE_CURRENT_LIST_DIR}/../../interfaces" ABSOLUTE)    # interfaces
get_filename_component(LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../libs" ABSOLUTE)                   # libs
get_filename_component(EXTERN_DIR "${CMAKE_CURRENT_LIST_DIR}/../../extern" ABSOLUTE)            # extern 


# Output directory:
if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE PATH "Archive output directory" FORCE) # .a .lib
endif()
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE PATH "Library output directory" FORCE) # .so .dylib .dll
endif()
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE PATH "Runtime output directory" FORCE) # .exe
endif()
# ---------------------------------------------------



# ------------------- File Lists --------------------
# *.* makes sure to only include files and not folders.
file(GLOB COMPUTE_FILES "src/compute/*.*")
source_group("Compute" FILES ${COMPUTE_FILES})

file(GLOB CONTEXT_FILES "src/context/*.*")
source_group("Context" FILES ${CONTEXT_FILES})

file(GLOB GPU_RESOURCE_POOLS_FILES "src/gpuResourcePools/*.*")
source_group("Gpu Resource Pools" FILES ${GPU_RESOURCE_POOLS_FILES})

file(GLOB GPU_RESOURCES_FILES "src/gpuResources/*.*")
source_group("Gpu Resources" FILES ${GPU_RESOURCES_FILES})

file(GLOB BUFFERS_FILES "src/gpuResources/buffers/*.*")
source_group("Gpu Resources/Buffers" FILES ${BUFFERS_FILES})

file(GLOB SAMPLERS_FILES "src/gpuResources/samplers/*.*")
source_group("Gpu Resources/Samplers" FILES ${SAMPLERS_FILES})

file(GLOB TEXTURES_FILES "src/gpuResources/textures/*.*")
source_group("Gpu Resources/Textures" FILES ${TEXTURES_FILES})

file(GLOB TEXTURE_1D_FILES "src/gpuResources/textures/texture1d/*.*")
source_group("Gpu Resources/Textures/Texture1d" FILES ${TEXTURE_1D_FILES})

file(GLOB TEXTURE_2D_FILES "src/gpuResources/textures/texture2d/*.*")
source_group("Gpu Resources/Textures/Texture2d" FILES ${TEXTURE_2D_FILES})

file(GLOB TEXTURE_3D_FILES "src/gpuResources/textures/texture3d/*.*")
source_group("Gpu Resources/Textures/Texture3d" FILES ${TEXTURE_3D_FILES})

file(GLOB TEXTURE_CUBE_FILES "src/gpuResources/textures/textureCube/*.*")
source_group("Gpu Resources/Textures/TextureCube" FILES ${TEXTURE_CUBE_FILES})

file(GLOB PIPELINES_FILES "src/pipelines/*.*")
source_group("Pipelines" FILES ${PIPELINES_FILES})

file(GLOB PUSH_CONSTANTS_FILES "src/pushConstants/*.*")
source_group("Push Constants" FILES ${PUSH_CONSTANTS_FILES})

file(GLOB RENDERER_FILES "src/renderer/*.*")
source_group("Renderer" FILES ${RENDERER_FILES})

file(GLOB RENDER_PASSES_FILES "src/renderPasses/*.*")
source_group("Render Passes" FILES ${RENDER_PASSES_FILES})

file(GLOB TYPES_FILES "src/types/*.*")
source_group("Types" FILES ${TYPES_FILES})

file(GLOB UTILITY_FILES "src/utility/*.*")
source_group("Utility" FILES ${UTILITY_FILES})

file(GLOB COMMON_FILES "${COMMON_DIR}/*.*")
source_group("Commen" FILES ${COMMON_FILES})

file(GLOB GUI_INTERFACE_FILES "${INTERFACES_DIR}/gui/*")
source_group("Gui Interface" FILES ${GUI_INTERFACE_FILES})

file(GLOB RENDERER_INTERFACE_FILES "${INTERFACES_DIR}/renderer/*.*")
source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

file(GLOB WINDOW_INTERFACE_FILES "${INTERFACES_DIR}/window/*.*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
if (VULKAN_RENDERER_BACKEND_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED
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
        ${COMMON_FILES}
        ${GUI_INTERFACE_FILES}
        ${RENDERER_INTERFACE_FILES}
        ${WINDOW_INTERFACE_FILES})
else()
    add_library(${PROJECT_NAME} STATIC
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
        ${COMMON_FILES}
        ${GUI_INTERFACE_FILES}
        ${RENDERER_INTERFACE_FILES}
        ${WINDOW_INTERFACE_FILES})
endif()

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
    ${COMMON_DIR}
    ${INTERFACES_DIR}/gui
    ${INTERFACES_DIR}/renderer
    ${INTERFACES_DIR}/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
# Path to vulkan library source directory:
get_filename_component(VULKAN_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_LIBRARY_ROOT_PATH="${VULKAN_LIBRARY_ROOT_PATH}")

# Library compilation logic:
if(VULKAN_RENDERER_BACKEND_BUILD_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VULKAN_RENDERER_BACKEND_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_RENDERER_BACKEND_STATIC)
endif()
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberAssetLoader:
if (NOT TARGET EmberAssetLoader)
    add_subdirectory("${LIBS_DIR}/assetLoader" EmberAssetLoader)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberAssetLoader)

# EmberLogger:
if (NOT TARGET EmberLogger)
    set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib.")
    add_subdirectory("${LIBS_DIR}/logger" EmberLogger)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberLogger)

# EmberMath:
if (NOT TARGET EmberMath)
    add_subdirectory("${LIBS_DIR}/math" EmberMath)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberMath)

# EmberProfiler:
if (NOT TARGET EmberProfiler)
    add_subdirectory("${LIBS_DIR}/profiler" EmberProfiler)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberProfiler)

# EmberTaskSystem:
if (NOT TARGET EmberTaskSystem)
    add_subdirectory("${LIBS_DIR}/taskSystem" EmberTaskSystem)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberTaskSystem)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# Vulkan (dynamicly linked):
find_package(Vulkan REQUIRED)           # required to be installed
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
    
# Vulkan Memory Allocator: (header only)
target_include_directories(${PROJECT_NAME} PUBLIC "${EXTERN_DIR}/vma/include")

# SPIRV-Reflect:
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE) # enable static lib compilation for SPIRV-Reflect
add_subdirectory("${EXTERN_DIR}/SPIRV-Reflect" "${CMAKE_BINARY_DIR}/SPIRV-Reflect_build")
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