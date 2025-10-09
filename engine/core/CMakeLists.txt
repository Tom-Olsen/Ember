# Ember/engine/core/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(EmberCore)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_CORE_PATH "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)               # engine core
get_filename_component(CORE_SHADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../shaders" ABSOLUTE)    # core shaders
get_filename_component(BACKENDS_DIR "${CMAKE_CURRENT_LIST_DIR}/../backends" ABSOLUTE)       # backends
get_filename_component(COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}/../common" ABSOLUTE)           # commom
get_filename_component(INTERFACES_DIR "${CMAKE_CURRENT_LIST_DIR}/../interfaces" ABSOLUTE)   # interfaces
get_filename_component(LIBS_DIR ${CMAKE_CURRENT_LIST_DIR}/../libs ABSOLUTE)                 # libs

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directors for add_executable(MyExe ...)     => .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/engine.h:
file(GLOB ENGINE_FILE "${ENGINE_CORE_PATH}/src/emberEngine.h")
source_group("/" FILES ${ENGINE_FILE})

# src/applications/*.*
file(GLOB APPLICATIONS_FILES "${ENGINE_CORE_PATH}/src/applications/*.*")
source_group("Applications" FILES ${APPLICATIONS_FILES})
 
# src/components/*.*:
file(GLOB COMPONENTS_FILES "${ENGINE_CORE_PATH}/src/components/*.*")
source_group("Components" FILES ${COMPONENTS_FILES})
 
## src/editorWindows/*.*:
#file(GLOB EDITOR_WINDOWS_FILES "${ENGINE_CORE_PATH}/src/editorWindows/*.*")
#source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
 
## src/editor/*.*:
#file(GLOB EDITOR_FILES "${ENGINE_CORE_PATH}/src/editor/*.*")
#source_group("Editors" FILES ${EDITOR_FILES})
 
# src/entityComponentSystem/*.*:
file(GLOB ENTITY_COMPONENT_SYSTEM_FILES "${ENGINE_CORE_PATH}/src/entityComponentSystem/*.*")
source_group("Entity Component System" FILES ${ENTITY_COMPONENT_SYSTEM_FILES})

# src/eventSystem/*.*:
file(GLOB EVENT_SYSTEM_FILES "${ENGINE_CORE_PATH}/src/eventSystem/*.*")
source_group("Event System" FILES ${EVENT_SYSTEM_FILES})

# src/gpuResources/*.*:
file(GLOB GPU_RESOURCES_FILES "${ENGINE_CORE_PATH}/src/gpuResources/*.*")
source_group("Gpu Resources" FILES ${GPU_RESOURCES_FILES})
 
# src/gui/*.*:
file(GLOB GUI_FILES "${ENGINE_CORE_PATH}/src/gUi/*.*")
source_group("Gui" FILES ${GUI_FILES})
 
# src/managers/*.*:
file(GLOB MANAGERS_FILES "${ENGINE_CORE_PATH}/src/managers/*.*")
source_group("Managers" FILES ${MANAGERS_FILES})
 
# src/physics/*.*:
file(GLOB PHYSICS_FILES "${ENGINE_CORE_PATH}/src/physics/*.*")
source_group("Physics" FILES ${PHYSICS_FILES})
 
# src/renderer/*.*:
file(GLOB RENDERER_FILES "${ENGINE_CORE_PATH}/src/renderer/*.*")
source_group("Renderer" FILES ${RENDERER_FILES})
 
# src/utility/*.*:
file(GLOB UTILITY_FILES "${ENGINE_CORE_PATH}/src/utility/*.*")
source_group("Utility" FILES ${UTILITY_FILES})

# src/window/*.*:
file(GLOB WINDOW_FILES "${ENGINE_CORE_PATH}/src/window/*.*")
source_group("Window" FILES ${WINDOW_FILES})

# ../../common/*.*:
file(GLOB COMMON_FILES "{COMMON_DIR}/*.*")
source_group("Commen" FILES ${COMMON_FILES})

# ../../interfaces/gui/*.*:
file(GLOB GUI_INTERFACE_FILES "${INTERFACES_DIR}/gui/*.*")
source_group("Gui Interface" FILES ${GUI_INTERFACE_FILES})
 
# ../../interfaces/renderer/*.*:
file(GLOB RENDERER_INTERFACE_FILES "${INTERFACES_DIR}/renderer/*.*")
source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

# ../../interfaces/window/*.*:
file(GLOB WINDOW_INTERFACE_FILES "${INTERFACES_DIR}/window/*.*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------
 
 
 
# ----------------- Create Library ------------------
add_library(${PROJECT_NAME} STATIC
    ${ENGINE_FILE}
    ${APPLICATIONS_FILES}
    ${COMPONENTS_FILES}
    #${EDITOR_FILES}
    #${EDITOR_WINDOWS_FILES}
    ${EVENT_SYSTEM_FILES}
    ${GPU_RESOURCES_FILES}
    ${GUI_FILES}
    ${ENTITY_COMPONENT_SYSTEM_FILES}
    ${MANAGERS_FILES}
    ${PHYSICS_FILES}
    ${RENDERER_FILES}
    ${UTILITY_FILES}
    ${WINDOW_FILES}
    ${COMMON_FILES}
    ${GUI_INTERFACE_FILES}
    ${RENDERER_INTERFACE_FILES}
    ${WINDOW_INTERFACE_FILES})
 
# Source subdirectories:
target_include_directories(${PROJECT_NAME}
    PUBLIC ${CORE_SHADERS_DIR}/src # needed for .h files from shaders, which contian constans for c++ and hlsl
    PUBLIC ${ENGINE_CORE_PATH}/src
    PUBLIC ${ENGINE_CORE_PATH}/src/applications
    PUBLIC ${ENGINE_CORE_PATH}/src/components
    #PUBLIC ${ENGINE_CORE_PATH}/src/editorWindows
    #PUBLIC ${ENGINE_CORE_PATH}/src/editor
    PUBLIC ${ENGINE_CORE_PATH}/src/entityComponentSystem
    PUBLIC ${ENGINE_CORE_PATH}/src/eventSystem
    PUBLIC ${ENGINE_CORE_PATH}/src/gpuResources
    PUBLIC ${ENGINE_CORE_PATH}/src/gui
    PUBLIC ${ENGINE_CORE_PATH}/src/managers
    PUBLIC ${ENGINE_CORE_PATH}/src/physics
    PUBLIC ${ENGINE_CORE_PATH}/src/renderer
    PUBLIC ${ENGINE_CORE_PATH}/src/utility
    PUBLIC ${ENGINE_CORE_PATH}/src/window
    PUBLIC ${ENGINE_CORE_PATH}/../common
    PUBLIC ${ENGINE_CORE_PATH}/../interfaces/gui
    PUBLIC ${ENGINE_CORE_PATH}/../interfaces/renderer
    PUBLIC ${ENGINE_CORE_PATH}/../interfaces/window)
# ---------------------------------------------------
 
 
 
# ---------------------- Macros ---------------------
target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_CORE_PATH="${ENGINE_CORE_PATH}")
target_compile_definitions(${PROJECT_NAME} PUBLIC CORE_SHADERS_DIR="${CORE_SHADERS_DIR}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberAssetLoader:
if (NOT TARGET EmberAssetLoader)
    add_subdirectory("${LIBS_DIR}/assetLoader" EmberAssetLoader)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberAssetLoader)

# EmberLogger:
if (NOT TARGET EmberLogger)
    set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib." FORCE)
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

# Shaders:
if (NOT TARGET ShaderCompiler)
    add_subdirectory(${CORE_SHADERS_DIR} ShaderCompiler)
endif()
add_dependencies(${PROJECT_NAME} ShaderCompiler)
# ---------------------------------------------------



# ------------------ Link Backends ------------------
# Null Window:
if (NOT TARGET nullWindowBackend)
    add_subdirectory("${BACKENDS_DIR}/nullWindow" nullWindowBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC nullWindowBackend)

# SDL Window:
if (NOT TARGET sdlWindowBackend)
    add_subdirectory("${BACKENDS_DIR}/sdlWindow" sdlWindowBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC sdlWindowBackend)

# Null Gui:
if (NOT TARGET nullGuiBackend)
    add_subdirectory("${BACKENDS_DIR}/nullGui" nullGuiBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC nullGuiBackend)

# ImGui SDL Vulkan:
if (NOT TARGET imGuiSdlVulkanBackend)
    add_subdirectory("${BACKENDS_DIR}/imGuiSdlVulkan" imGuiSdlVulkanBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC imGuiSdlVulkanBackend)

# Vulkan Renderer:
if (NOT TARGET vulkanRendererBackend)
    add_subdirectory("${BACKENDS_DIR}/vulkanRenderer" vulkanRendererBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC vulkanRendererBackend)
# ---------------------------------------------------