# Ember/engine/backends/imGuiSdlVulkan/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(ImGuiSdlVulkanBackend)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed to link static libraries into this dynamic library.

# Static vs Shared library option:
option(IMGUI_SDL_VULKAN_BACKEND_BUILD_SHARED "Build ${PROJECT_NAME} as a shared library" ON) # default = dynamic

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
file(GLOB SOURCE_FILES "src/*")
source_group("Source Files" FILES ${SOURCE_FILES})

# ../../common/*:
file(GLOB COMMON_INTERFACE_FILES "../../common/*.*")
source_group("Commen" FILES ${COMMON_INTERFACE_FILES})

# ../../interfaces/editor/*:
file(GLOB EDITOR_INTERFACE_FILES "../../interfaces/editor/*")
source_group("Editor Interface" FILES ${EDITOR_INTERFACE_FILES})

# ../../interfaces/gui/*:
file(GLOB GUI_INTERFACE_FILES "../../interfaces/gui/*")
source_group("Gui Interface" FILES ${GUI_INTERFACE_FILES})

# ../../interfaces/renderer/*:
file(GLOB RENDERER_INTERFACE_FILES "../../interfaces/renderer/*")
source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

# ../../interfaces/window/*:
file(GLOB WINDOW_INTERFACE_FILES "../../interfaces/window/*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})

set(IMGUI_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../../extern/imgui/imgui.h"
    "${CMAKE_CURRENT_LIST_DIR}/../../extern/imgui/imgui.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../extern/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../extern/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../extern/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../extern/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../extern/imgui/backends/imgui_impl_sdl3.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../extern/imgui/backends/imgui_impl_vulkan.cpp")
source_group("ImGui" FILES ${IMGUI_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
if (IMGUI_SDL_VULKAN_BACKEND_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED
        ${SOURCE_FILES}
        ${IMGUI_FILES}
        ${COMMON_INTERFACE_FILES}
        ${EDITOR_INTERFACE_FILES}
        ${GUI_INTERFACE_FILES}
        ${RENDERER_INTERFACE_FILES}
        ${WINDOW_INTERFACE_FILES})
else()
    add_library(${PROJECT_NAME} STATIC
        ${SOURCE_FILES}
        ${IMGUI_FILES}
        ${COMMON_INTERFACE_FILES}
        ${EDITOR_INTERFACE_FILES}
        ${GUI_INTERFACE_FILES}
        ${RENDERER_INTERFACE_FILES}
        ${WINDOW_INTERFACE_FILES})
endif()


target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/window
    ${CMAKE_CURRENT_SOURCE_DIR}/../../extern/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../extern/imgui/backends)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
if(IMGUI_SDL_VULKAN_BACKEND_BUILD_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_SDL_VULKAN_BACKEND_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_SDL_VULKAN_BACKEND_STATIC)
endif()
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberLogger:
if (NOT TARGET EmberLogger)
    set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib.")
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
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# SDL (CMakeList target):
if (NOT TARGET SDL3::SDL3-shared)
    set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as a static library" FORCE)
    set(SDL_SHARED ON CACHE BOOL "Build SDL3 as a shared library" FORCE)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../extern/SDL" "${CMAKE_BINARY_DIR}/SDL_build")
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3-shared)

# Vulkan (dynamically linked):
find_package(Vulkan REQUIRED)           # required to be installed
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
# ---------------------------------------------------