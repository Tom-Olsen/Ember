# Ember/engine/backends/nullWindow/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(NullWindowBackend)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed to link static libraries into this dynamic library.

# Static vs Shared library option:
option(NULL_WINDOW_BACKEND_BUILD_SHARED "Build ${PROJECT_NAME} as a shared library" ON) # default = dynamic

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
file(GLOB COMMON_FILES "../../common/*.*")
source_group("Commen" FILES ${COMMON_FILES})

# ../../interfaces/window/*:
file(GLOB WINDOW_INTERFACE_FILES "../../interfaces/window/*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
if (NULL_WINDOW_BACKEND_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED
        ${SOURCE_FILES}
        ${COMMON_FILES}
        ${WINDOW_INTERFACE_FILES})
else()
    add_library(${PROJECT_NAME} STATIC
        ${SOURCE_FILES}
        ${COMMON_FILES}
        ${WINDOW_INTERFACE_FILES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
if(NULL_WINDOW_BACKEND_BUILD_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NULL_WINDOW_BACKEND_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC NULL_WINDOW_BACKEND_STATIC)
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