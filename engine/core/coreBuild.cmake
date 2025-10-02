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
# Null Window Backend:
add_subdirectory("${CORE_ROOT_DIR}/../backends/nullWindow" "${CMAKE_BINARY_DIR}/nullWindowBackend_build")

# SDL Window Backend:
add_subdirectory("${CORE_ROOT_DIR}/../backends/sdlWindow" "${CMAKE_BINARY_DIR}/sdlWindowBackend_build")

# Vulkan Renderer Backend:
add_subdirectory("${CORE_ROOT_DIR}/../backends/vulkanRenderer" "${CMAKE_BINARY_DIR}/vulkanRendererBackend_build")
# ---------------------------------------------------



# Function which specifies which variant of the engine core to build:
function(build_ember_core DEAR_IMGUI_ENABLED EDITOR_ENABLED WINDOW_ENABLED)
    # ------------------- File Lists --------------------
    # Shader directory:
    set(SHADER_DIR "${CORE_ROOT_DIR}/../shaders")

    # src/engine.h:
    file(GLOB ENGINE_FILE "${CORE_ROOT_DIR}/src/emberEngine.h")
    source_group("/" FILES ${ENGINE_FILE})
    
    # src/components/*:
    file(GLOB COMPONENTS_FILES "${CORE_ROOT_DIR}/src/components/*")
    source_group("Components" FILES ${COMPONENTS_FILES})
    
    # src/dearImGui/*:
    file(GLOB DEAR_IMGUI_FILES "${CORE_ROOT_DIR}/src/dearImGUi/*")
    source_group("Dear ImGui" FILES ${DEAR_IMGUI_FILES})
    
    ## src/editorWindows/*:
    #file(GLOB EDITOR_WINDOWS_FILES "${CORE_ROOT_DIR}/src/editorWindows/*")
    #source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
    
    ## src/editor/*:
    #file(GLOB EDITOR_FILES "${CORE_ROOT_DIR}/src/editor/*")
    #source_group("Editors" FILES ${EDITOR_FILES})
    
    # src/entityComponentSystem/*:
    file(GLOB ENTITY_COMPONENT_SYSTEM_FILES "${CORE_ROOT_DIR}/src/entityComponentSystem/*")
    source_group("Game Object System" FILES ${ENTITY_COMPONENT_SYSTEM_FILES})

    # src/eventSystem/*:
    file(GLOB EVENT_SYSTEM_FILES "${CORE_ROOT_DIR}/src/eventSystem/*")
    source_group("Event System" FILES ${EVENT_SYSTEM_FILES})

    # src/gpuResources/*:
    file(GLOB GPU_RESOURCES_FILES "${CORE_ROOT_DIR}/src/gpuResources/*")
    source_group("Gpu Resources" FILES ${GPU_RESOURCES_FILES})
    
    # src/managers/*:
    file(GLOB MANAGERS_FILES "${CORE_ROOT_DIR}/src/managers/*")
    source_group("Managers" FILES ${MANAGERS_FILES})
    
    # src/physics/*:
    file(GLOB PHYSICS_FILES "${CORE_ROOT_DIR}/src/physics/*")
    source_group("Physics" FILES ${PHYSICS_FILES})
    
    # src/renderer/*:
    file(GLOB RENDERER_FILES "${CORE_ROOT_DIR}/src/renderer/*")
    source_group("Renderer" FILES ${RENDERER_FILES})
    
    # src/utility/*:
    file(GLOB UTILITY_FILES "${CORE_ROOT_DIR}/src/utility/*")
    source_group("Utility" FILES ${UTILITY_FILES})

    # src/window/*:
    file(GLOB WINDOW_FILES "${CORE_ROOT_DIR}/src/window/*")
    source_group("Window" FILES ${WINDOW_FILES})

    # ../../common/*:
    file(GLOB COMMON_INTERFACE_FILES "../../common/*.*")
    source_group("Commen" FILES ${COMMON_INTERFACE_FILES})
    
    # ../../interfaces/renderer/*:
    file(GLOB RENDERER_INTERFACE_FILES "${CORE_ROOT_DIR}/../interfaces/renderer/*")
    source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

    # ../../interfaces/window/*:
    file(GLOB WINDOW_INTERFACE_FILES "${CORE_ROOT_DIR}/../interfaces/window/*")
    source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
    # ---------------------------------------------------
    
    
    
    # ----------------- Create Library ------------------
    add_library(${PROJECT_NAME} STATIC
        ${ENGINE_FILE}
        ${COMPONENTS_FILES}
        ${DEAR_IMGUI_FILES}
        #${EDITOR_FILES}
        #${EDITOR_WINDOWS_FILES}
        ${EVENT_SYSTEM_FILES}
        ${GPU_RESOURCES_FILES}
        ${ENTITY_COMPONENT_SYSTEM_FILES}
        ${MANAGERS_FILES}
        ${PHYSICS_FILES}
        ${RENDERER_FILES}
        ${UTILITY_FILES}
        ${WINDOW_FILES}
        ${COMMON_INTERFACE_FILES}
        ${RENDERER_INTERFACE_FILES}
        ${WINDOW_INTERFACE_FILES})
    
    # Source subdirectories:
    target_include_directories(${PROJECT_NAME}
        PUBLIC ${SHADER_DIR}/src # needed for .h files from shaders, which contian constans for c++ and hlsl
        PUBLIC ${CORE_ROOT_DIR}/src
        PUBLIC ${CORE_ROOT_DIR}/src/components
        PUBLIC ${CORE_ROOT_DIR}/src/dearImGui
        #PUBLIC ${CORE_ROOT_DIR}/src/editorWindows
        #PUBLIC ${CORE_ROOT_DIR}/src/editor
        PUBLIC ${CORE_ROOT_DIR}/src/entityComponentSystem
        PUBLIC ${CORE_ROOT_DIR}/src/eventSystem
        PUBLIC ${CORE_ROOT_DIR}/src/gpuResources
        PUBLIC ${CORE_ROOT_DIR}/src/managers
        PUBLIC ${CORE_ROOT_DIR}/src/physics
        PUBLIC ${CORE_ROOT_DIR}/src/renderer
        PUBLIC ${CORE_ROOT_DIR}/src/utility
        PUBLIC ${CORE_ROOT_DIR}/src/window
        PUBLIC ${CORE_ROOT_DIR}/../common
        PUBLIC ${CORE_ROOT_DIR}/../interfaces/renderer
        PUBLIC ${CORE_ROOT_DIR}/../interfaces/window)

    # Add this file as custom target, so that it is always visible in the IDE:
    add_custom_target(BuildSystem SOURCES ${CORE_ROOT_DIR}/coreBuild.cmake)
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
    # ---------------------------------------------------



    # -------------- Link Custom Libraries --------------
    # EmberAssetLoader:
    if (NOT TARGET EmberAssetLoader)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../libs/assetLoader" EmberAssetLoader)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberAssetLoader)

    # EmberLogger:
    if (NOT TARGET EmberLogger)
        add_subdirectory("${CORE_ROOT_DIR}/../libs/logger" EmberLogger)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberLogger)

    # EmberMath:
    if (NOT TARGET EmberMath)
        add_subdirectory("${CORE_ROOT_DIR}/../libs/math" EmberMath)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberMath)

    # EmberProfiler:
    if (NOT TARGET EmberProfiler)
        add_subdirectory("${CORE_ROOT_DIR}/../libs/profiler" EmberProfiler)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberProfiler)

    # EmberTaskSystem:
    if (NOT TARGET EmberTaskSystem)
        add_subdirectory("${CORE_ROOT_DIR}/../libs/taskSystem" EmberTaskSystem)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberTaskSystem)

    # Shaders:
    if (NOT TARGET ShaderCompiler)
        add_subdirectory(${SHADER_DIR} ShaderCompiler)
    endif()
    add_dependencies(${PROJECT_NAME} ShaderCompiler)

    # Null Window Backend:
    target_link_libraries(${PROJECT_NAME} PUBLIC nullWindowBackend)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../backends/nullWindow/src)

    # SDL Window Backend:
    target_link_libraries(${PROJECT_NAME} PUBLIC sdlWindowBackend)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../backends/sdlWindow/src)

    # Vulkan Renderer Backend:
    target_link_libraries(${PROJECT_NAME} PUBLIC vulkanRendererBackend)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_ROOT_DIR}/../backends/vulkanRenderer/src)
    # ---------------------------------------------------
endfunction()