# Ember/engine/core/coreBuild.cmake
# This file defines the function build_ember_core(TARGET_NAME DEAR_IMGUI_ENABLED EDITOR_ENABLED WINDOW_ENABLED).
# The three folders editorMode/gameMode/headlessMode contain the CMakeLists.txt with the correct flags
# to build the core in the desired mode. This makes sure that only the desired core is shown is the VS solution.



# ------------------- Project Setup ------------------
# Choose SHARED (.dll) or STATIC (.lib) library (currently only static supported):
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(EmberCore)    # name of project group, not individual targets.

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directory of this file:
get_filename_component(CORE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin") # target directory for add_executable(...)           => .exe
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
# ---------------------------------------------------



# ------ Add Shared dependencies Subdirectories -----
# EmberMath:
add_subdirectory("${CORE_ROOT_DIR}/../libs/math" EmberMath)

# Shaders:
set(SHADER_DIR "${CORE_ROOT_DIR}/../shaders")
add_subdirectory(${SHADER_DIR} ShaderCompiler)

# Null Window Backend:
add_subdirectory("${CORE_ROOT_DIR}/../backends/nullWindow" "${CMAKE_BINARY_DIR}/nullWindowBackend_build")

# SDL Window Backend:
add_subdirectory("${CORE_ROOT_DIR}/../backends/sdlWindow" "${CMAKE_BINARY_DIR}/sdlWindowBackend_build")

# spdlog:
add_subdirectory("${CORE_ROOT_DIR}/../extern/spdlog" "${CMAKE_BINARY_DIR}/spdlog_build")

# SPIRV-Reflect:
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE)
add_subdirectory("${CORE_ROOT_DIR}/../extern/SPIRV-Reflect" "${CMAKE_BINARY_DIR}/SPIRV-Reflect_build")
# ---------------------------------------------------



# Function which specifies which variant of the engine core to build:
function(build_ember_core DEAR_IMGUI_ENABLED EDITOR_ENABLED WINDOW_ENABLED)
    # ------------------- File Lists --------------------
    # src/engine.h:
    file(GLOB ENGINE_FILE "${CORE_ROOT_DIR}/src/emberEngine.h")
    source_group("/" FILES ${ENGINE_FILE})
    
    # src/buffers/*:
    file(GLOB BUFFERS_FILES "${CORE_ROOT_DIR}/src/buffers/*")
    source_group("Buffers" FILES ${BUFFERS_FILES})
    
    # src/components/*:
    file(GLOB COMPONENTS_FILES "${CORE_ROOT_DIR}/src/components/*")
    source_group("Components" FILES ${COMPONENTS_FILES})
    
    # src/compute/*:
    file(GLOB COMPUTE_FILES "${CORE_ROOT_DIR}/src/compute/*")
    source_group("Compute" FILES ${COMPUTE_FILES})
    
    # src/coreSystems/*:
    file(GLOB CORE_SYSTEM_FILES "${CORE_ROOT_DIR}/src/coreSystems/*")
    source_group("Core Systems" FILES ${CORE_SYSTEM_FILES})
    
    # src/dearImGui/*:
    file(GLOB DEAR_IMGUI_FILES "${CORE_ROOT_DIR}/src/dearImGUi/*")
    source_group("Dear ImGui" FILES ${DEAR_IMGUI_FILES})
    
    ## src/editorWindows/*:
    #file(GLOB EDITOR_WINDOWS_FILES "${CORE_ROOT_DIR}/src/editorWindows/*")
    #source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
    
    # src/editor/*:
    file(GLOB EDITOR_FILES "${CORE_ROOT_DIR}/src/editor/*")
    source_group("Editors" FILES ${EDITOR_FILES})

    # src/eventSystem/*:
    file(GLOB EVENT_SYSTEM_FILES "${CORE_ROOT_DIR}/src/eventSystem/*")
    source_group("Event System" FILES ${EVENT_SYSTEM_FILES})
    
    # src/gameObjectSystem/*:
    file(GLOB GAME_OBJECT_SYSTEM_FILES "${CORE_ROOT_DIR}/src/gameObjectSystem/*")
    source_group("Game Object System" FILES ${GAME_OBJECT_SYSTEM_FILES})
    
    # src/gpuResources/*:
    file(GLOB GPU_RESOURCES_FILES "${CORE_ROOT_DIR}/src/gpuResources/*")
    source_group("GPU Resources" FILES ${GPU_RESOURCES_FILES})
    
    # src/graphics/*:
    file(GLOB GRAPHICS_FILES "${CORE_ROOT_DIR}/src/graphics/*")
    source_group("Graphics" FILES ${GRAPHICS_FILES})
    
    # src/managers/*:
    file(GLOB MANAGERS_FILES "${CORE_ROOT_DIR}/src/managers/*")
    source_group("Managers" FILES ${MANAGERS_FILES})
    
    # src/physics/*:
    file(GLOB PHYSICS_FILES "${CORE_ROOT_DIR}/src/physics/*")
    source_group("Physics" FILES ${PHYSICS_FILES})
    
    # src/textures/*:
    file(GLOB TEXTURES_FILES "${CORE_ROOT_DIR}/src/textures/*")
    source_group("Textures" FILES ${TEXTURES_FILES})
    
    # src/utility/*:
    file(GLOB UTILITY_FILES "${CORE_ROOT_DIR}/src/utility/*")
    source_group("Utility" FILES ${UTILITY_FILES})
    
    # src/vulkanBackend/*:
    file(GLOB VULKAN_BACKEND_FILES "${CORE_ROOT_DIR}/src/vulkanBackend/*")
    source_group("Vulkan Backend" FILES ${VULKAN_BACKEND_FILES})
    
    # src/vulkanBackend/pipelines/*:
    file(GLOB VULKAN_PIPELINES_FILES "${CORE_ROOT_DIR}/src/vulkanBackend/pipelines/*")
    source_group("Vulkan Backend\\Pipelines" FILES ${VULKAN_PIPELINES_FILES})
    
    # src/vulkanBackend/pushConstants/*:
    file(GLOB VULKAN_PUSH_CONSTANTS_FILES "${CORE_ROOT_DIR}/src/vulkanBackend/pushConstants/*")
    source_group("Vulkan Backend\\Push Constants" FILES ${VULKAN_PUSH_CONSTANTS_FILES})
    
    # src/vulkanBackend/renderPasses/*:
    file(GLOB VULKAN_RENDER_PASSES_FILES "${CORE_ROOT_DIR}/src/vulkanBackend/renderPasses/*")
    source_group("Vulkan Backend\\Render Passes" FILES ${VULKAN_RENDER_PASSES_FILES})
    
    # src/vulkanBackend/utility/*:
    file(GLOB VULKAN_UTILITY_FILES "${CORE_ROOT_DIR}/src/vulkanBackend/utility/*")
    file(GLOB CPP_HLSL_HEADERS "${CORE_ROOT_DIR}/src/shaders/*.h")   # grab .h files form shaders, which contian constans for c++ and hlsl
    list(APPEND VULKAN_UTILITY_FILES ${CPP_HLSL_HEADERS})
    source_group("Vulkan Backend\\Utility" FILES ${VULKAN_UTILITY_FILES})

    # src/window/*:
    file(GLOB WINDOW_FILES "${CORE_ROOT_DIR}/src/window/*")
    source_group("Window" FILES ${WINDOW_FILES})
    
    # ../../interfaces/utility/*:
    file(GLOB UTILITY_INTERFACE_FILES "${CORE_ROOT_DIR}/../interfaces/utility/*")
    source_group("Utility Interface" FILES ${UTILITY_INTERFACE_FILES})

    # ../../interfaces/window/*:
    file(GLOB WINDOW_INTERFACE_FILES "${CORE_ROOT_DIR}/../interfaces/window/*")
    source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
    # ---------------------------------------------------
    
    
    
    # ----------------- Create Library ------------------
    add_library(${PROJECT_NAME} STATIC
        ${ENGINE_FILE}
        ${BUFFERS_FILES}
        ${COMPONENTS_FILES}
        ${COMPUTE_FILES}
        ${CORE_SYSTEM_FILES}
        ${DEAR_IMGUI_FILES}
        ${EDITOR_WINDOWS_FILES}
        ${EVENT_SYSTEM_FILES}
        ${EDITOR_FILES}
        ${GAME_OBJECT_SYSTEM_FILES}
        ${GPU_RESOURCES_FILES}
        ${GRAPHICS_FILES}
        ${MANAGERS_FILES}
        ${PHYSICS_FILES}
        ${TEXTURES_FILES}
        ${UTILITY_FILES}
        ${VULKAN_BACKEND_FILES}
        ${VULKAN_PIPELINES_FILES}
        ${VULKAN_PUSH_CONSTANTS_FILES}
        ${VULKAN_RENDER_PASSES_FILES}
        ${VULKAN_UTILITY_FILES}
        ${WINDOW_FILES}
        ${UTILITY_INTERFACE_FILES}
        ${WINDOW_INTERFACE_FILES})
    
    # Source subdirectories:
    target_include_directories(${PROJECT_NAME}
        PUBLIC ${CORE_ROOT_DIR}/src
        PUBLIC ${CORE_ROOT_DIR}/src/buffers
        PUBLIC ${CORE_ROOT_DIR}/src/components
        PUBLIC ${CORE_ROOT_DIR}/src/compute
        PUBLIC ${CORE_ROOT_DIR}/src/coreSystems
        PUBLIC ${CORE_ROOT_DIR}/src/dearImGui
        #PUBLIC ${CORE_ROOT_DIR}/src/editorWindows
        PUBLIC ${CORE_ROOT_DIR}/src/editor
        PUBLIC ${CORE_ROOT_DIR}/src/eventSystem
        PUBLIC ${CORE_ROOT_DIR}/src/gameObjectSystem
        PUBLIC ${CORE_ROOT_DIR}/src/gpuResources
        PUBLIC ${CORE_ROOT_DIR}/src/graphics
        PUBLIC ${CORE_ROOT_DIR}/src/managers
        PUBLIC ${CORE_ROOT_DIR}/src/physics
        PUBLIC ${SHADER_DIR}/src # needed for .h files from shaders, which contian constans for c++ and hlsl
        PUBLIC ${CORE_ROOT_DIR}/src/textures
        PUBLIC ${CORE_ROOT_DIR}/src/utility
        PUBLIC ${CORE_ROOT_DIR}/src/vulkanBackend
        PUBLIC ${CORE_ROOT_DIR}/src/vulkanBackend/pipelines
        PUBLIC ${CORE_ROOT_DIR}/src/vulkanBackend/pushConstants
        PUBLIC ${CORE_ROOT_DIR}/src/vulkanBackend/renderPasses
        PUBLIC ${CORE_ROOT_DIR}/src/vulkanBackend/utility
        PUBLIC ${CORE_ROOT_DIR}/src/window
        PUBLIC ${CORE_ROOT_DIR}/../interfaces/utility
        PUBLIC ${CORE_ROOT_DIR}/../interfaces/window)

        get_target_property(_srcs ${PROJECT_NAME} SOURCES)
message(STATUS "Target ${PROJECT_NAME} sources: ${_srcs}")
    # ---------------------------------------------------
    
    
    
    # ---------------------- Macros ---------------------
    # Path to engine:
    get_filename_component(ENGINE_CORE_PATH "${CORE_ROOT_DIR}" ABSOLUTE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_CORE_PATH="${ENGINE_CORE_PATH}")

    # Path to shaders:
    get_filename_component(CORE_SHADERS_DIR ${SHADER_DIR} ABSOLUTE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CORE_SHADERS_DIR="${CORE_SHADERS_DIR}")

    # Enable/Disabled Imgui:
    if(DEAR_IMGUI_ENABLED)
        target_compile_definitions(${PROJECT_NAME} PUBLIC "DEAR_IMGUI_ENABLED")
    endif()

    # Enable/Disabled Editor:
    if(EDITOR_ENABLED)
        target_compile_definitions(${PROJECT_NAME} PUBLIC "EDITOR_ENABLED")
    endif()

    # Enable/Disabled SDL:
    if(WINDOW_ENABLED)
        target_compile_definitions(${PROJECT_NAME} PUBLIC "WINDOW_ENABLED")
    endif()

    # Enable utf-8 encoding for microsoft visual studio: (TODO: fix files so i can remove this)
    if (MSVC)
        target_compile_options(${PROJECT_NAME} PUBLIC /utf-8)
    endif()
    # ---------------------------------------------------



    # -------------- Link Custom Libraries --------------
    # EmberMath:
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberMath)

    # EmberTaskSystem:
    if (NOT TARGET EmberTaskSystem)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../libs/taskSystem" EmberTaskSystem)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberTaskSystem)

    # Null Window Backend:
    target_link_libraries(${PROJECT_NAME} PUBLIC nullWindowBackend)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../backends/nullWindow/src)

    # SDL Window Backend:
    target_link_libraries(${PROJECT_NAME} PUBLIC sdlWindowBackend)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../backends/sdlWindow/src)
    # ---------------------------------------------------



    # ------------- Link External Libraries -------------
    # Shaders:
    add_dependencies(${PROJECT_NAME} ShaderCompiler)
    
    # spdlog (CMakeList target):
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../extern/spdlog/include)
    
    # SPIRV-Reflect (CMakeList target):
    set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE) # enable static lib compilation for SPIRV-Reflect
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../extern/SPIRV-Reflect)
    target_link_libraries(${PROJECT_NAME} PUBLIC spirv-reflect-static)
    
    # Vulkan (dynamicly linked):
    find_package(Vulkan REQUIRED)           # required to be installed
    target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
    
    # Vulkan Memory Allocator: (header only)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../extern/vma/include)
    
    # Taskflow: (header only)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../extern/taskflow)
    # ---------------------------------------------------
endfunction()