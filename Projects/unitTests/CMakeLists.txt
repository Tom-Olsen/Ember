# Ember/Projects/fluidDynamics/CMakeLists.txt
cmake_minimum_required(VERSION 3.31)
project(UnitTests)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory("../../engine" EmberEngine)

# Enable googletest:
add_subdirectory(libs/googletest)
enable_testing()


file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.h" "${PROJECT_SOURCE_DIR}/src/*.cpp")
source_group("Source Files" FILES ${SOURCE_FILES})



add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC EmberEngine PRIVATE gtest gtest_main)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})