# Ember/projects/fluidDynamics/unitTests/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.30)
project(FluidDynamicsUnitTests)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(FLUID_DYNAMICS_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)          # fluidDynamics
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../engine" ABSOLUTE)     # engine
get_filename_component(ENGINE_APPLICATIONS_DIR "${ENGINE_DIR}/applications" ABSOLUTE)       # applications
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)                   # extern
get_filename_component(PROJECT_SHADERS_DIR "${FLUID_DYNAMICS_DIR}/shaders" ABSOLUTE)        # shaders

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .so .dylib .dll, must be next to .exe on windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB MAIN_FILE "src/main.cpp")
source_group("/" FILES ${MAIN_FILE})

file(GLOB TEST_FILES "src/test*.cpp")
source_group("Tests" FILES ${TEST_FILES})

file(GLOB 2D_CPU_FILES "${FLUID_DYNAMICS_DIR}/src/2d/cpu/*.*")
source_group("2D/cpu" FILES ${2D_CPU_FILES})

file(GLOB 2D_GPU_FILES "${FLUID_DYNAMICS_DIR}/src/2d/gpu/*.*")
source_group("2D/gpu" FILES ${2D_GPU_FILES})

file(GLOB 3D_FILES "${FLUID_DYNAMICS_DIR}/src/3d/*.*")
source_group("3D" FILES ${3D_FILES})

file(GLOB UTILITY_FILES "${FLUID_DYNAMICS_DIR}/src/utility/*.*")
source_group("Utility" FILES ${UTILITY_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE}
    ${TEST_FILES}
    ${2D_CPU_FILES}
    ${2D_GPU_FILES}
    ${3D_FILES}
    ${UTILITY_FILES})

# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${FLUID_DYNAMICS_DIR}/src
    ${FLUID_DYNAMICS_DIR}/src/2d/cpu
    ${FLUID_DYNAMICS_DIR}/src/2d/gpu
    ${FLUID_DYNAMICS_DIR}/src/3d
    ${FLUID_DYNAMICS_DIR}/src/utility)

# Add project root path as a macro:
get_filename_component(PROJECT_ROOT_PATH "${FLUID_DYNAMICS_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# ---------------------- Macros ---------------------
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



# ------------- Link External Libraries -------------
# Googletest:
if(USE_EXTERNAL_GTEST)
    target_link_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main)
else()
    add_subdirectory("${ENGINE_EXTERN_DIR}/googletest" googletest-distribution)
    target_link_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main)
endif()

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
# ---------------------------------------------------