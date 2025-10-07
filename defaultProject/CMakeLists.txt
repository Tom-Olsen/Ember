# Ember/defaultProject/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.31)
project(DefaultProject)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib STATIC ...) => .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directory for add_library(MyLib SHARED ...) => .so .dylib .dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}") # target directors for add_executable(MyExe ...)     => .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/main.cpp:
file(GLOB MAIN_FILE "src/main.cpp")
source_group("/" FILES ${MAIN_FILE})

# src/scenes/*:
file(GLOB SCENE_FILES "src/scenes/*")
source_group("Scenes" FILES ${SCENE_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE} ${SCENE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/scenes)

# Add project root path as a macro:
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
# Ember:
add_subdirectory("../engine/core" EmberCore)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberCore)
# ---------------------------------------------------