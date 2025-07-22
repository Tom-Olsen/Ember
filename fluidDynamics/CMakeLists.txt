# Ember/fluidDynamics/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.31)
project(FluidDynamics)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
# ---------------------------------------------------



# ------------------- File Lists --------------------
# src/2d/*:
file(GLOB 2D_CPU_FILES "${PROJECT_SOURCE_DIR}/src/2d/cpu/*")
source_group("2D\\cpu" FILES ${2D_CPU_FILES})
file(GLOB 2D_GPU_FILES "${PROJECT_SOURCE_DIR}/src/2d/gpu/*")
source_group("2D\\gpu" FILES ${2D_GPU_FILES})

# src/3d/*:
file(GLOB 3D_FILES "${PROJECT_SOURCE_DIR}/src/3d/*")
source_group("3D" FILES ${3D_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} src/main.cpp
${MAIN_FILE}
${2D_CPU_FILES}
${2D_GPU_FILES}
${3D_FILES})

# Add src subfolders as include directories:
target_include_directories(${PROJECT_NAME}
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/2d/cpu
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/2d/gpu
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/3d
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add project root path as a compile definition
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
add_subdirectory("../Engine" EmberEngine)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberEngine)

# Shaders:
add_subdirectory("src/shaders")
add_dependencies(${PROJECT_NAME} CompileFluidDynamicsShaders)
# ---------------------------------------------------






