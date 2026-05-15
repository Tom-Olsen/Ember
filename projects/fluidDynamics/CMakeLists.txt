# Ember/projects/fluidDynamics/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.30)
project(FluidDynamics)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine" ABSOLUTE)        # engine
get_filename_component(ENGINE_APPLICATIONS_DIR "${ENGINE_DIR}/applications" ABSOLUTE)       # applications
get_filename_component(PROJECT_SHADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/shaders" ABSOLUTE)    # shaders

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .so .dylib .dll, must be next to .exe on windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB MAIN_FILE "src/main.cpp")
source_group("/" FILES ${MAIN_FILE})

file(GLOB 2D_CPU_FILES "src/2d/cpu/*.*")
source_group("2D/cpu" FILES ${2D_CPU_FILES})

file(GLOB 2D_GPU_FILES "src/2d/gpu/*.*")
source_group("2D/gpu" FILES ${2D_GPU_FILES})

file(GLOB 3D_FILES "src/3d/*.*")
source_group("3D" FILES ${3D_FILES})

file(GLOB UTILITY_FILES "src/utility/*.*")
source_group("Utility" FILES ${UTILITY_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE}
    ${2D_CPU_FILES}
    ${2D_GPU_FILES}
    ${3D_FILES}
    ${UTILITY_FILES})

# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/2d/cpu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/2d/gpu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utility)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_SHADERS_DIR="${PROJECT_SHADERS_DIR}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# Ember Game Debug Application:
add_subdirectory("${ENGINE_APPLICATIONS_DIR}/emberGameDebugApp" EmberGameDebugApp)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberGameDebugApp)

# Shaders:
add_subdirectory(${PROJECT_SHADERS_DIR} FluidDynamicsShaderCompiler)
add_dependencies(${PROJECT_NAME} FluidDynamicsShaderCompiler)
# ---------------------------------------------------