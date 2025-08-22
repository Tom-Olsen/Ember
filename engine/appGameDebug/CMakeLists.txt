# Ember/engine/appGameDebug/CMakeLists.txt



# ------------------- Project Setup ------------------
# Choose SHARED (.dll) or STATIC (.lib) library (currently only static supported):
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(Application)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/*:
file(GLOB SRC_FILES "src/*")
source_group("Source Files" FILES ${SRC_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${PROJECT_NAME} ${PROJECT_TYPE} ${SRC_FILES})

# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
# EmberCore:
add_subdirectory("../core/gameDebugMode" EmberCore)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberCore)
# ---------------------------------------------------