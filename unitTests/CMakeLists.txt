# Ember/unitTests/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.31)
project(UnitTests)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
# ---------------------------------------------------




# ------------------- File Lists --------------------
file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.h")
source_group("Source Files" FILES ${SOURCE_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES})

# Add project root path as a macro:
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# ---------------- Enable Googletest ----------------
add_subdirectory(libs/googletest)
enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
# Ember:
add_subdirectory("../Engine" EmberEngine)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberEngine)

# Googletest:
target_link_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main)
# ---------------------------------------------------