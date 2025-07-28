# Ember/editor/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.31)
project(Editor)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/editorWindows/*:
file(GLOB EDITOR_WINDOWS_FILES "${PROJECT_SOURCE_DIR}/src/editorWindows/*")
source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} src/main.cpp
${EDITOR_WINDOWS_FILES})

# Add project root path as a macro:
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")

target_include_directories(${PROJECT_NAME}
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/editorWindows)
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
# Ember:
add_subdirectory("../engine" EmberEngine)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberEngine)
# ---------------------------------------------------