# Ember/applications/emberGameDebugApp/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(EmberGameDebugApp)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(APPLICATION_PATH "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)                       # applicatiojn
get_filename_component(ENGINE_CORE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../engine/core" ABSOLUTE)     # engine core
get_filename_component(CORE_SHADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/shaders" ABSOLUTE)  # core shaders
get_filename_component(BACKENDS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/backends" ABSOLUTE)     # backends
get_filename_component(COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/common" ABSOLUTE)         # commom
get_filename_component(INTERFACES_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/interfaces" ABSOLUTE) # interfaces
get_filename_component(LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/libs" ABSOLUTE)             # libs

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directors for add_executable(MyExe ...)     => .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/*:
file(GLOB SOURCE_FILES "${APPLICATION_PATH}/src/*")
source_group("Source Files" FILES ${SOURCE_FILES})
# ---------------------------------------------------
 
 
 
# ----------------- Create Library ------------------
add_library(${PROJECT_NAME} STATIC
    ${SOURCE_FILES})
 
# Source subdirectories:
target_include_directories(${PROJECT_NAME}
    PUBLIC ${APPLICATION_PATH}/src)
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

# Vulkan Renderer:
if (NOT TARGET VulkanRendererBackend)
    add_subdirectory("${BACKENDS_DIR}/vulkanRenderer" VulkanRendererBackend)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanRendererBackend)
# ---------------------------------------------------