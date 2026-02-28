# Ember/engine/libs/bufferLayout/unitTests/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.30)
project(EmberBufferLayoutUnitTests)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)    # engine
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)               # libs
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)           # extern
get_filename_component(ENGINE_SHADERS_DIR "${ENGINE_DIR}/shaders" ABSOLUTE)         # shaders

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

file(GLOB TEST_FILES "src/test*.cpp")
source_group("Tests" FILES ${TEST_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE} ${TEST_FILES} ${SPIRV_TEST})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_SHADERS_DIR="${ENGINE_SHADERS_DIR}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberBufferLayout:
if (NOT TARGET EmberBufferLayout)
    add_subdirectory("${ENGINE_LIBS_DIR}/bufferLayout" EmberBufferLayout)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberBufferLayout)

# EmberLogger:
if (NOT TARGET EmberLogger)
    set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib.")
    add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberLogger)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# Googletest:
if(USE_EXTERNAL_GTEST)
    target_link_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main)
else()
    if (NOT TARGET googletest-distribution)
        add_subdirectory("${ENGINE_EXTERN_DIR}/googletest" googletest-distribution)
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main)
endif()

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
# ---------------------------------------------------