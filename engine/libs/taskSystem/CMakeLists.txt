
# Ember/engine/libs/taskSystem/CMakeLists.txt



# ------------------- Project Setup ------------------
# Choose SHARED (.dll) or STATIC (.lib) library (currently only static supported):
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(EmberTaskSystem)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed for ubuntu to link this into a shared library.

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
file(GLOB SRC_FILES "src/*")
source_group("Source Files" FILES ${SRC_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${PROJECT_NAME} ${PROJECT_TYPE} ${SRC_FILES})

# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# Taskflow: (header only)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../extern/taskflow)
# ---------------------------------------------------