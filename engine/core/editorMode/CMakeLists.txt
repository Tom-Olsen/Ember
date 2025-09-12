# Ember/engine/core/editorMode/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(CoreEditorMode)

# Static vs Shared library option:
#option(BUILD_SHARED_LIBS "Build ${PROJECT_NAME} as a shared library" ON) # default ON

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directors for add_executable(MyExe ...)     => .exe
# ---------------------------------------------------



# Include the core build function:
include("${CMAKE_CURRENT_LIST_DIR}/../coreBuild.cmake")

# Build editor mode variant:
# Function arguments: target DEAR_IMGUI_ENABLED EDITOR_ENABLED WINDOW_ENABLED
build_ember_core(ON ON ON)