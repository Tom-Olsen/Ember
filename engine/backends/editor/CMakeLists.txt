# Ember/engine/backends/editor/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(editorBackend)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed to link static libraries into this dynamic library.

# Static vs Shared library option:
option(EDITOR_BACKEND_BUILD_SHARED "Build ${PROJECT_NAME} as a shared library" ON) # default = dynamic

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

# src/editorWindows/*:
file(GLOB EDITOR_WINDOWS_FILES "src/editorWindows/*")
source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
if (EDITOR_BACKEND_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED
        ${SOURCE_FILES}
        ${EDITOR_WINDOWS_FILES})
else()
    add_library(${PROJECT_NAME} STATIC
        ${SOURCE_FILES}
        ${EDITOR_WINDOWS_FILES})
endif()

# Only expose public files to consumer:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/editorWindows)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
# Library compilation logic:
if(EDITOR_BACKEND_BUILD_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE EDITOR_BACKEND_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC EDITOR_BACKEND_STATIC)
endif()
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberMath:
if (NOT TARGET EmberMath)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../libs/math" EmberMath)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberMath)
# ---------------------------------------------------