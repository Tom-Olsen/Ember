# Ember/engine/backends/nullWindow/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.31)
project(nullWindowBackend)

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

# ../../interfaces/window/*:
file(GLOB WINDOW_INTERFACE_FILES "../../interfaces/window/*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${WINDOW_INTERFACE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NULL_WINDOW_BACKEND_EXPORTS)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NULL_WINDOW_BACKEND_STATIC)
endif()
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberMath:
if (NOT TARGET EmberMath)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../libs/math" EmberMath)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberMath)
# ---------------------------------------------------