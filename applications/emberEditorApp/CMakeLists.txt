# Ember/applications/emberEditorApp/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(EmberEditorApp)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(APPLICATION_PATH "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)                       # application
get_filename_component(ENGINE_CORE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../engine/core" ABSOLUTE)     # engine core
get_filename_component(BACKENDS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/backends" ABSOLUTE)     # backends

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
# src/*:
file(GLOB SOURCE_FILES "${APPLICATION_PATH}/src/*")
source_group("Source Files" FILES ${SOURCE_FILES})

# src/editorWindows/*:
file(GLOB EDITOR_WINDOWS_FILES "src/editorWindows/*")
source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
# ---------------------------------------------------
 
 
 
# ----------------- Create Library ------------------
add_library(${PROJECT_NAME} STATIC
    ${SOURCE_FILES}
    ${EDITOR_WINDOWS_FILES})
 
# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${APPLICATION_PATH}/src
    ${APPLICATION_PATH}/src/editorWindows)
# ---------------------------------------------------
 
 
 
# ---------------------- Macros ---------------------
target_compile_definitions(${PROJECT_NAME} PUBLIC APPLICATION_PATH="${APPLICATION_PATH}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberCore:
if (NOT TARGET EmberCore)
    add_subdirectory(${ENGINE_CORE_PATH} EmberCore)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberCore)
# ---------------------------------------------------



# ------------------ Link Backends ------------------
# Vulkan Renderer:
if (NOT TARGET VulkanRendererBackend)
    add_subdirectory("${BACKENDS_DIR}/vulkanRenderer" VulkanRendererBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanRendererBackend)

# Null Window:
if (NOT TARGET NullWindowBackend)
    add_subdirectory("${BACKENDS_DIR}/nullWindow" NullWindowBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC NullWindowBackend)

# SDL Window:
if (NOT TARGET SdlWindowBackend)
    add_subdirectory("${BACKENDS_DIR}/sdlWindow" SdlWindowBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC SdlWindowBackend)

# Null Gui:
if (NOT TARGET NullGuiBackend)
    add_subdirectory("${BACKENDS_DIR}/nullGui" NullGuiBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC NullGuiBackend)

# ImGui SDL Vulkan:
if (NOT TARGET ImGuiSdlVulkanBackend)
    add_subdirectory("${BACKENDS_DIR}/imGuiSdlVulkan" ImGuiSdlVulkanBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC ImGuiSdlVulkanBackend)
# ---------------------------------------------------