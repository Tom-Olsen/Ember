# Ember/engine/core/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(EmberCore)

# Static vs Shared library option:
option(EMBER_CORE_BUILD_SHARED "Build ${PROJECT_NAME} as a shared library" ON) # default = dynamic

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}/../common" ABSOLUTE)            # commom
get_filename_component(ENGINE_CORE_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)                        # core
get_filename_component(ENGINE_EXTERN_DIR "${CMAKE_CURRENT_LIST_DIR}/../extern" ABSOLUTE)            # extern
get_filename_component(ENGINE_INTERFACES_DIR "${CMAKE_CURRENT_LIST_DIR}/../interfaces" ABSOLUTE)    # interfaces
get_filename_component(ENGINE_LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/../libs" ABSOLUTE)                # libs
get_filename_component(ENGINE_RESOURCES_DIR "${CMAKE_CURRENT_LIST_DIR}/../resources" ABSOLUTE)      # resources
get_filename_component(ENGINE_SHADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../shaders" ABSOLUTE)          # shaders

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
file(GLOB ENGINE_FILE "${ENGINE_CORE_DIR}/src/*.*")
source_group("/" FILES ${ENGINE_FILE})
 
file(GLOB EDITOR_FILES "${ENGINE_CORE_DIR}/src/editor/*.*")
source_group("Editor" FILES ${EDITOR_FILES})

file(GLOB EVENT_SYSTEM_FILES "${ENGINE_CORE_DIR}/src/eventSystem/*.*")
source_group("Event System" FILES ${EVENT_SYSTEM_FILES})

file(GLOB GPU_RESOURCES_FILES "${ENGINE_CORE_DIR}/src/gpuResources/*.*")
source_group("Gpu Resources" FILES ${GPU_RESOURCES_FILES})
 
file(GLOB GUI_FILES "${ENGINE_CORE_DIR}/src/gui/*.*")
source_group("Gui" FILES ${GUI_FILES})
 
file(GLOB MANAGERS_FILES "${ENGINE_CORE_DIR}/src/managers/*.*")
source_group("Managers" FILES ${MANAGERS_FILES})
 
file(GLOB PHYSICS_FILES "${ENGINE_CORE_DIR}/src/physics/*.*")
source_group("Physics" FILES ${PHYSICS_FILES})
 
file(GLOB RENDERER_FILES "${ENGINE_CORE_DIR}/src/renderer/*.*")
source_group("Renderer" FILES ${RENDERER_FILES})
 
file(GLOB UTILITY_FILES "${ENGINE_CORE_DIR}/src/utility/*.*")
source_group("Utility" FILES ${UTILITY_FILES})

file(GLOB WINDOW_FILES "${ENGINE_CORE_DIR}/src/window/*.*")
source_group("Window" FILES ${WINDOW_FILES})

file(GLOB COMMON_FILES "${ENGINE_COMMON_DIR}/*.*")
source_group("Common" FILES ${COMMON_FILES})

file(GLOB GUI_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/gui/*.*")
source_group("Gui Interface" FILES ${GUI_INTERFACE_FILES})
 
file(GLOB RENDERER_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/renderer/*.*")
source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

file(GLOB WINDOW_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/window/*.*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------
 
 
 
# ----------------- Create Library ------------------
if (EMBER_CORE_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED
        ${ENGINE_FILE}
        ${EDITOR_FILES}
        ${EVENT_SYSTEM_FILES}
        ${GPU_RESOURCES_FILES}
        ${GUI_FILES}
        ${MANAGERS_FILES}
        ${PHYSICS_FILES}
        ${RENDERER_FILES}
        ${UTILITY_FILES}
        ${WINDOW_FILES}
        ${COMMON_FILES}
        ${GUI_INTERFACE_FILES}
        ${RENDERER_INTERFACE_FILES}
        ${WINDOW_INTERFACE_FILES})
else()
    add_library(${PROJECT_NAME} STATIC
        ${ENGINE_FILE}
        ${EDITOR_FILES}
        ${EVENT_SYSTEM_FILES}
        ${GPU_RESOURCES_FILES}
        ${GUI_FILES}
        ${MANAGERS_FILES}
        ${PHYSICS_FILES}
        ${RENDERER_FILES}
        ${UTILITY_FILES}
        ${WINDOW_FILES}
        ${COMMON_FILES}
        ${GUI_INTERFACE_FILES}
        ${RENDERER_INTERFACE_FILES}
        ${WINDOW_INTERFACE_FILES})
endif()

# Source subdirectories:
target_include_directories(${PROJECT_NAME}
    PUBLIC ${ENGINE_SHADERS_DIR}/src # needed for .h files from shaders, which contian constans for c++ and hlsl
    PUBLIC ${ENGINE_CORE_DIR}/src
    PUBLIC ${ENGINE_CORE_DIR}/src/editor
    PUBLIC ${ENGINE_CORE_DIR}/src/eventSystem
    PUBLIC ${ENGINE_CORE_DIR}/src/gpuResources
    PUBLIC ${ENGINE_CORE_DIR}/src/gui
    PUBLIC ${ENGINE_CORE_DIR}/src/managers
    PUBLIC ${ENGINE_CORE_DIR}/src/physics
    PUBLIC ${ENGINE_CORE_DIR}/src/renderer
    PUBLIC ${ENGINE_CORE_DIR}/src/utility
    PUBLIC ${ENGINE_CORE_DIR}/src/window
    PUBLIC ${ENGINE_COMMON_DIR}
    PUBLIC ${ENGINE_INTERFACES_DIR}/gui
    PUBLIC ${ENGINE_INTERFACES_DIR}/renderer
    PUBLIC ${ENGINE_INTERFACES_DIR}/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_CORE_DIR="${ENGINE_CORE_DIR}")
target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_RESOURCES_DIR="${ENGINE_RESOURCES_DIR}")
target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_SHADERS_DIR="${ENGINE_SHADERS_DIR}")

# Library compilation logic:
if(EMBER_CORE_BUILD_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE EMBER_CORE_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC EMBER_CORE_STATIC)
endif()
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberAssetLoader:
if (NOT TARGET EmberAssetLoader)
    add_subdirectory("${ENGINE_LIBS_DIR}/assetLoader" EmberAssetLoader)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberAssetLoader)

# EmberLogger:
if (NOT TARGET EmberLogger)
    set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib." FORCE)
    add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberLogger)

# EmberMath:
if (NOT TARGET EmberMath)
    add_subdirectory("${ENGINE_LIBS_DIR}/math" EmberMath)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberMath)

# EmberProfiler:
if (NOT TARGET EmberProfiler)
    add_subdirectory("${ENGINE_LIBS_DIR}/profiler" EmberProfiler)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberProfiler)

# EmberTaskSystem:
if (NOT TARGET EmberTaskSystem)
    add_subdirectory("${ENGINE_LIBS_DIR}/taskSystem" EmberTaskSystem)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberTaskSystem)

# Shaders:
if (NOT TARGET ShaderCompiler)
    add_subdirectory(${ENGINE_SHADERS_DIR} ShaderCompiler)
endif()
add_dependencies(${PROJECT_NAME} ShaderCompiler)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
#EnTT:
set(ENTT_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
if (NOT TARGET EnTT::EnTT)
    add_subdirectory("${ENGINE_EXTERN_DIR}/EnTT" "${CMAKE_BINARY_DIR}/EnTT_build")
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
# ---------------------------------------------------