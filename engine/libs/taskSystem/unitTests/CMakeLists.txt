
# Ember/engine/libs/taskSystem/unitTests/CMakeLists.txt



# ------------------- Project Setup ------------------
# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(EmberTaskSystemUnitTests)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)    # engine
get_filename_component(ENGINE_BACKENDS_DIR "${ENGINE_DIR}/backends" ABSOLUTE)       # backends
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)               # libs
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)           # extern

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .so .dylib .dll, must be next to .exe on windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB MAIN_FILE "src/main.cpp")
source_group("/" FILES ${MAIN_FILE})

file(GLOB HEADER_FILES "src/*.h")
source_group("/" FILES ${HEADER_FILES})

file(GLOB TEST_FILES "src/test*.cpp")
source_group("Tests" FILES ${TEST_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE} ${TEST_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberMath:
add_subdirectory("${ENGINE_LIBS_DIR}/taskSystem" EmberTaskSystem)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberTaskSystem)
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