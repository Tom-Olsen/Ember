# Ember/engine/core/CMakeLists.txt



# ------------------- Project Setup ------------------
# Choose SHARED (.dll) or STATIC (.lib) library (currently only static supported):
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(EmberEngine)    # name of project group, not individual targets.

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin") # target directory for add_executable(...)           => .exe
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
# ---------------------------------------------------


# Function which specifies which variant onf the engine to build:
function(build_ember_engine TARGET_NAME DEAR_IMGUI_ENABLED EDITOR_ENABLED WINDOW_ENABLED)
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
    
    # Dear Imgui files:
    if(DEAR_IMGUI_ENABLED)
        set(DEAR_IMGUI_FILES
            "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGui.h"
            "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGuiFlags.h"
            "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGuiEnabled.h"
            "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGuiEnabled.cpp")
        set(IMGUI_FILES
            libs/imgui/imgui.h
            libs/imgui/imgui.cpp
            libs/imgui/imgui_demo.cpp
            libs/imgui/imgui_draw.cpp
            libs/imgui/imgui_tables.cpp
            libs/imgui/imgui_widgets.cpp
            libs/imgui/backends/imgui_impl_sdl3.cpp
            libs/imgui/backends/imgui_impl_vulkan.cpp)
    else()
        set(DEAR_IMGUI_FILES
            "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGui.h"
            "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGuiFlags.h"
            "${PROJECT_SOURCE_DIR}/src/dearImGui/dearImGuiDisabled.h")
    endif()
    source_group("Dear ImGui" FILES ${DEAR_IMGUI_FILES})
    source_group("ImGui Demo" FILES ${IMGUI_FILES})
    
    # src/editorWindows/*:
    file(GLOB EDITOR_WINDOWS_FILES "${PROJECT_SOURCE_DIR}/src/editorWindows/*")
    source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
    
    # src/editor/*:
    if(EDITOR_ENABLED)
        set(EDITOR_FILES
        "${PROJECT_SOURCE_DIR}/src/editor/editor.h"
        "${PROJECT_SOURCE_DIR}/src/editor/editorEnabled.h"
        "${PROJECT_SOURCE_DIR}/src/editor/editorEnabled.cpp"
        "${PROJECT_SOURCE_DIR}/src/editor/editorWindow.h"
        "${PROJECT_SOURCE_DIR}/src/editor/editorWindow.cpp")
    else()
        set(EDITOR_FILES
        "${PROJECT_SOURCE_DIR}/src/editor/editor.h"
        "${PROJECT_SOURCE_DIR}/src/editor/editorDisabled.h"
        "${PROJECT_SOURCE_DIR}/src/editor/editorWindow.h"
        "${PROJECT_SOURCE_DIR}/src/editor/editorWindow.cpp")
    endif()
    source_group("Editors" FILES ${EDITOR_FILES})

    # src/eventSystem/*:
    if(WINDOW_ENABLED)
        set(EVENT_SYSTEM_FILES
        "${PROJECT_SOURCE_DIR}/src/eventSystem/eventSystem.h"
        "${PROJECT_SOURCE_DIR}/src/eventSystem/eventSystemEnabled.h"
        "${PROJECT_SOURCE_DIR}/src/eventSystem/eventSystemEnabled.cpp")
    else()
        set(EVENT_SYSTEM_FILES
        "${PROJECT_SOURCE_DIR}/src/eventSystem/eventSystem.h"
        "${PROJECT_SOURCE_DIR}/src/eventSystem/eventSystemDisabled.h")
    endif()
    source_group("Event System" FILES ${EVENT_SYSTEM_FILES})
    
    # src/gameObjectSystem/*:
    file(GLOB GAME_OBJECT_SYSTEM_FILES "${PROJECT_SOURCE_DIR}/src/gameObjectSystem/*")
    source_group("Game Object System" FILES ${GAME_OBJECT_SYSTEM_FILES})
    
    # src/gpuResources/*:
    file(GLOB GPU_RESOURCES_FILES "${PROJECT_SOURCE_DIR}/src/gpuResources/*")
    source_group("GPU Resources" FILES ${GPU_RESOURCES_FILES})
    
    # src/graphics/*:
    file(GLOB GRAPHICS_FILES "${PROJECT_SOURCE_DIR}/src/graphics/*")
    source_group("Graphics" FILES ${GRAPHICS_FILES})
    
    # src/managers/*:
    file(GLOB MANAGERS_FILES "${PROJECT_SOURCE_DIR}/src/managers/*")
    source_group("Managers" FILES ${MANAGERS_FILES})
    
    # src/physics/*:
    file(GLOB PHYSICS_FILES "${PROJECT_SOURCE_DIR}/src/physics/*")
    source_group("Physics" FILES ${PHYSICS_FILES})
    
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

    # src/window/*:
    if(WINDOW_ENABLED)
        set(WINDOW_FILES
        "${PROJECT_SOURCE_DIR}/src/window/sdlWindow.h"
        "${PROJECT_SOURCE_DIR}/src/window/sdlWindowEnabled.h"
        "${PROJECT_SOURCE_DIR}/src/window/sdlWindowEnabled.cpp")
    else()
        set(WINDOW_FILES
        "${PROJECT_SOURCE_DIR}/src/window/sdlWindow.h"
        "${PROJECT_SOURCE_DIR}/src/window/sdlWindowDisabled.h")
    endif()
    source_group("Window" FILES ${WINDOW_FILES})
    # ---------------------------------------------------
    
    
    
    # ----------------- Create Library ------------------
    add_library(${TARGET_NAME} STATIC
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
        ${IMGUI_FILES}
        ${MANAGERS_FILES}
        ${PHYSICS_FILES}
        ${TEXTURES_FILES}
        ${UTILITY_FILES}
        ${VULKAN_BACKEND_FILES}
        ${VULKAN_PIPELINES_FILES}
        ${VULKAN_PUSH_CONSTANTS_FILES}
        ${VULKAN_RENDER_PASSES_FILES}
        ${VULKAN_UTILITY_FILES}
        ${WINDOW_FILES})

    # Add dependency on shader outputs:
    #target_sources(${TARGET_NAME} PRIVATE ${SPIRV_OUTPUTS})
    
    # Source subdirectories:
    target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/buffers
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/components
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/compute
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/coreSystems
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/dearImGui
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/editorWindows
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/eventSystem
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gameObjectSystem
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gpuResources
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/managers
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/physics
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders # needed for .h files form shaders, which contian constans for c++ and hlsl
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/textures
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/utility
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend/pipelines
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend/pushConstants
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend/renderPasses
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanBackend/utility
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/window)
    
    # Add project root path as a macro:
    #get_filename_component(ENGINE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
    #target_compile_definitions(${TARGET_NAME} PUBLIC ENGINE_ROOT_PATH="${ENGINE_ROOT_PATH}")

    # Macros:
    get_filename_component(ENGINE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
    target_compile_definitions(${TARGET_NAME} PUBLIC ENGINE_ROOT_PATH="${ENGINE_ROOT_PATH}")
    if(DEAR_IMGUI_ENABLED)
        target_compile_definitions(${TARGET_NAME} PUBLIC "DEAR_IMGUI_ENABLED")
    endif()
    if(EDITOR_ENABLED)
        target_compile_definitions(${TARGET_NAME} PUBLIC "EDITOR_ENABLED")
    endif()
    if(WINDOW_ENABLED)
        target_compile_definitions(${TARGET_NAME} PUBLIC "WINDOW_ENABLED")
    endif()

    # Enable utf-8 encoding for microsoft visual studio: (TODO: fix files so i can remove this)
    if (MSVC)
        target_compile_options(${TARGET_NAME} PUBLIC /utf-8)
    endif()
    # ---------------------------------------------------
    
    
    
    # ------------------ Link Libraries -----------------
    # EmberMath:
    target_link_libraries(${TARGET_NAME} PUBLIC EmberMath)

    # Shaders:
    add_dependencies(${TARGET_NAME} CompileShaders)
    
    # Imgui:
    if(DEAR_IMGUI_ENABLED)
        target_include_directories(${TARGET_NAME} PUBLIC libs/imgui libs/imgui/backends)
    endif()
    
    # SDL (CMakeList target):
    if(WINDOW_ENABLED)
        set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)
        target_include_directories(${TARGET_NAME} PUBLIC libs/SDL/include)
        target_link_libraries(${TARGET_NAME} PUBLIC SDL3::SDL3-static)
        if(UNIX)
            target_link_libraries(${TARGET_NAME} PRIVATE pthread dl)
        endif(UNIX)
        if(WIN32)
            target_link_libraries(${TARGET_NAME} PRIVATE winmm dxguid)
        endif(WIN32)
    endif()
    
    # spdlog (CMakeList target):
    target_include_directories(${TARGET_NAME} PUBLIC libs/spdlog/include)
    
    # SPIRV-Reflect (CMakeList target):
    set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE) # enable static lib compilation for SPIRV-Reflect
    target_include_directories(${TARGET_NAME} PUBLIC libs/SPIRV-Reflect)
    target_link_libraries(${TARGET_NAME} PUBLIC spirv-reflect-static)
    
    # Vulkan (dynamicly linked):
    find_package(Vulkan REQUIRED)           # required to be installed
    target_include_directories(${TARGET_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${TARGET_NAME} PUBLIC ${Vulkan_LIBRARIES})
    
    # Vulkan Memory Allocator: (header only)
    target_include_directories(${TARGET_NAME} PUBLIC libs/vma/include)
    
    # Taskflow: (header only)
    target_include_directories(${TARGET_NAME} PUBLIC libs/taskflow)
    # ---------------------------------------------------
endfunction()



# ------ Add Shared dependencies Subdirectories -----
# EmberMath:
add_subdirectory("../math" EmberMath)
# Shaders:
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")
# SDL:
set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL")
# spdlog:
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog")
# SPIRV-Reflect:
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/SPIRV-Reflect")
# ---------------------------------------------------



# ------------------ Build Variants -----------------
# Function arguments: target DEAR_IMGUI_ENABLED EDITOR_ENABLED WINDOW_ENABLED
build_ember_engine(EmberEngineEditorMode ON ON ON)
build_ember_engine(EmberEngineGameMode OFF OFF ON)
build_ember_engine(EmberEngineHeadlessMode OFF OFF OFF)
# ---------------------------------------------------