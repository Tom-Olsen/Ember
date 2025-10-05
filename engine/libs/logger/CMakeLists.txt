
# Ember/engine/libs/logger/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(EmberLogger)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed to link static libraries into this dynamic library.

# Static vs Shared library option:
option(EMBER_LOGGER_BUILD_SHARED "Build ${PROJECT_NAME} as a shared library" OFF) # default = static

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directors for add_executable(MyExe ...) 
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/*:
file(GLOB SRC_FILES "src/*")
source_group("Source Files" FILES ${SRC_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
if (EMBER_LOGGER_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
else()
    add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
endif()

# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable utf-8 encoding for microsoft visual studio (needed for spdlog):
if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /utf-8)
endif()
# ---------------------------------------------------



# ---------------------- Macros ---------------------
# Library compilation logic:
if (EMBER_LOGGER_BUILD_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LOGGER_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC LOGGER_STATIC)
endif()
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# spdlog (CMakeList target):
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../extern/spdlog/include)
# ---------------------------------------------------