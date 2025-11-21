# Ember/projects/fluidDynamics/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.30)
project(FluidDynamics)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_APPLICATIONS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/applications" ABSOLUTE)  # applications
get_filename_component(ENGINE_EXTERN_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/extern" ABSOLUTE)              # extern
get_filename_component(PROJECT_SHADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/shaders" ABSOLUTE)                        # shaders

# Custom target name for unit tests:
set(TEST_PR_NAME ${PROJECT_NAME}UnitTests)

# Output directory:
if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE PATH "Archive output directory" FORCE) # .a .lib
endif()
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE PATH "Library output directory" FORCE) # .so .dylib .dll
endif()
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE PATH "Runtime output directory" FORCE) # .exe
endif()
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB MAIN_FILE "src/main.cpp")
source_group("/" FILES ${MAIN_FILE})

file(GLOB UNIT_TESTS_MAIN_FILE "src/unitTests.cpp")
source_group("/" FILES ${UNIT_TESTS_MAIN_FILE})

file(GLOB 2D_CPU_FILES "src/2d/cpu/*.*")
source_group("2D/cpu" FILES ${2D_CPU_FILES})

file(GLOB 2D_GPU_FILES "src/2d/gpu/*.*")
source_group("2D/gpu" FILES ${2D_GPU_FILES})

file(GLOB 3D_FILES "src/3d/*.*")
source_group("3D" FILES ${3D_FILES})

file(GLOB TESTS_FILES "src/tests/*.*")
source_group("Tests" FILES ${TESTS_FILES})

file(GLOB UTILITY_FILES "src/utility/*.*")
source_group("Utility" FILES ${UTILITY_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE}
    ${2D_CPU_FILES}
    ${2D_GPU_FILES}
    ${3D_FILES}
    ${UTILITY_FILES})
add_executable(${TEST_PR_NAME} ${UNIT_TESTS_MAIN_FILE}
    ${2D_CPU_FILES}
    ${2D_GPU_FILES}
    ${3D_FILES}
    ${TESTS_FILES}
    ${UTILITY_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/2d/cpu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/2d/gpu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utility)
target_include_directories(${TEST_PR_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/2d/cpu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/2d/gpu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tests
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utility)

# Add project root path as a compile definition
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
target_compile_definitions(${TEST_PR_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# ---------------------- Macros ---------------------
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_SHADERS_DIR="${PROJECT_SHADERS_DIR}")
target_compile_definitions(${TEST_PR_NAME} PUBLIC PROJECT_SHADERS_DIR="${PROJECT_SHADERS_DIR}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# Ember Game Debug Application:
if (NOT TARGET EmberGameDebugApp)
    add_subdirectory("${ENGINE_APPLICATIONS_DIR}/emberGameDebugApp" EmberGameDebugApp)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberGameDebugApp)
target_link_libraries(${TEST_PR_NAME} PUBLIC EmberGameDebugApp)

# Shaders:
if (NOT TARGET ProjectShaderCompiler)
    add_subdirectory(${PROJECT_SHADERS_DIR} ProjectShaderCompiler)
endif()
add_dependencies(${PROJECT_NAME} ProjectShaderCompiler)
add_dependencies(${TEST_PR_NAME} ProjectShaderCompiler)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# Googletest: (for unit tests executable only)
if (NOT TARGET googletest-distribution)
    add_subdirectory("${ENGINE_EXTERN_DIR}/googletest" googletest-distribution)
endif()
target_link_libraries(${TEST_PR_NAME} PRIVATE gtest gtest_main)
enable_testing()
add_test(NAME ${TEST_PR_NAME} COMMAND ${TEST_PR_NAME})
# ---------------------------------------------------