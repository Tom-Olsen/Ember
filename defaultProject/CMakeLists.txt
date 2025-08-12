# Ember/defaultProject/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.31)
project(DefaultProject)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} src/main.cpp)

# Add project root path as a macro:
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
# Ember:
#add_subdirectory("../engine" EmberEngineEditorMode)
#target_link_libraries(${PROJECT_NAME} PUBLIC EmberEngineEditorMode)
add_subdirectory("../engine" EmberEngineGameMode)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberEngineGameMode)
# ---------------------------------------------------