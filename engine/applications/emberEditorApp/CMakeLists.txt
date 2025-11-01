# Ember/applications/emberEditorApp/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(EmberEditorApp)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_APPLICATIONS_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)            # application
get_filename_component(ENGINE_BACKENDS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../backends" ABSOLUTE) # backends
get_filename_component(ENGINE_CORE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../core" ABSOLUTE)         # core
get_filename_component(ENGINE_LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../libs" ABSOLUTE)         # libs

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
file(GLOB SOURCE_FILES "${ENGINE_APPLICATIONS_DIR}/src/*.*")
source_group("Source Files" FILES ${SOURCE_FILES})

file(GLOB EDITOR_WINDOWS_FILES "src/editorWindows/*.*")
source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
# ---------------------------------------------------
 
 
 
# ----------------- Create Library ------------------
add_library(${PROJECT_NAME} STATIC
    ${SOURCE_FILES}
    ${EDITOR_WINDOWS_FILES})
 
# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${ENGINE_APPLICATIONS_DIR}/src
    ${ENGINE_APPLICATIONS_DIR}/src/editorWindows)
# ---------------------------------------------------
 
 
 
# ---------------------- Macros ---------------------
target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_APPLICATIONS_DIR="${ENGINE_APPLICATIONS_DIR}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberCore:
if (NOT TARGET EmberCore)
    add_subdirectory(${ENGINE_CORE_DIR} EmberCore)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberCore)

# EntityComponentSystem:
if (NOT TARGET EntityComponentSystem)
    add_subdirectory("${ENGINE_LIBS_DIR}/entityComponentSystem" EntityComponentSystem)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EntityComponentSystem)
# ---------------------------------------------------



# ------------------ Link Backends ------------------
# Vulkan Renderer:
if (NOT TARGET VulkanRendererBackend)
    add_subdirectory("${ENGINE_BACKENDS_DIR}/vulkanRenderer" VulkanRendererBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanRendererBackend)

# Null Window:
if (NOT TARGET NullWindowBackend)
    add_subdirectory("${ENGINE_BACKENDS_DIR}/nullWindow" NullWindowBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC NullWindowBackend)

# SDL Window:
if (NOT TARGET SdlWindowBackend)
    add_subdirectory("${ENGINE_BACKENDS_DIR}/sdlWindow" SdlWindowBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC SdlWindowBackend)

# Null Gui:
if (NOT TARGET NullGuiBackend)
    add_subdirectory("${ENGINE_BACKENDS_DIR}/nullGui" NullGuiBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC NullGuiBackend)

# ImGui SDL Vulkan:
if (NOT TARGET ImGuiSdlVulkanBackend)
    add_subdirectory("${ENGINE_BACKENDS_DIR}/imGuiSdlVulkan" ImGuiSdlVulkanBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC ImGuiSdlVulkanBackend)
# ---------------------------------------------------