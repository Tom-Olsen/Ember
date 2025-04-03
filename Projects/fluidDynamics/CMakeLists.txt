# Ember/Projects/fluidDynamics/CMakeLists.txt

cmake_minimum_required(VERSION 3.31)
project(FluidDynamics)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory("../../engine" EmberEngine)



file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.h" "${PROJECT_SOURCE_DIR}/src/*.cpp")
source_group("Source Files" FILES ${SOURCE_FILES})



add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC EmberEngine)



# Shaders:
add_subdirectory("src/shaders")
add_dependencies(${PROJECT_NAME} CompileFluidDynamicsShaders)


# Add project root path as a compile definition
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")