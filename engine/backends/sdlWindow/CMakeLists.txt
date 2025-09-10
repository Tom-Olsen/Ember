# Ember/engine/backends/sdlWindow/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(sdlWindowBackend)

# Static vs Shared library option:
option(BUILD_SHARED_LIBS "Build ${PROJECT_NAME} as a shared library" ON) # default ON

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directors for add_executable(MyExe ...)     => .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/*:
file(GLOB SOURCE_FILES "src/*")
source_group("Source Files" FILES ${SOURCE_FILES})

# ../../interfaces/math/*:
file(GLOB MATH_INTERFACE_FILES "../../interfaces/math/*")
source_group("Math Interface" FILES ${MATH_INTERFACE_FILES})

# ../../interfaces/window/*:
file(GLOB WINDOW_INTERFACE_FILES "../../interfaces/window/*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})

set(IMGUI_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../../libs/imgui/imgui.h"
    "${CMAKE_CURRENT_LIST_DIR}/../../libs/imgui/imgui.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../libs/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../libs/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../libs/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../libs/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../libs/imgui/backends/imgui_impl_sdl3.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../libs/imgui/backends/imgui_impl_vulkan.cpp")
source_group("ImGui" FILES ${IMGUI_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${IMGUI_FILES}
    ${MATH_INTERFACE_FILES}
    ${WINDOW_INTERFACE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/math
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/window
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/imgui/backends)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_WINDOW_BACKEND_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_WINDOW_BACKEND_STATIC)
endif()
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
# SDL (CMakeList target):
set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)
add_subdirectory("../../libs/SDL" "${CMAKE_BINARY_DIR}/SDL_build")
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3-static)

# Vulkan (dynamically linked):
find_package(Vulkan REQUIRED)           # required to be installed
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
# ---------------------------------------------------