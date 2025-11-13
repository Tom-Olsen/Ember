# Ember/projects/unitTests/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.30)
project(UnitTests)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_APPLICATIONS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/applications" ABSOLUTE)  # applications
get_filename_component(ENGINE_EXTERN_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/extern" ABSOLUTE)              # extern
get_filename_component(ENGINE_LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine/libs" ABSOLUTE)                  # libs

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

file(GLOB TESTS_FILES "src/tests/*.*")
source_group("Tests" FILES ${TESTS_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE}
    ${TESTS_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/tests)

# Add project root path as a macro:
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# Ember Headless Application:
#add_subdirectory("${ENGINE_APPLICATIONS_DIR}/emberHeadlessApp" EmberHeadlessApp)
#target_link_libraries(${PROJECT_NAME} PUBLIC EmberHeadlessApp)

# Ember Game Application:
add_subdirectory("${ENGINE_APPLICATIONS_DIR}/embergameApp" EmberGameApp)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberGameApp)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# Googletest:
if (NOT TARGET googletest-distribution)
    add_subdirectory("${ENGINE_EXTERN_DIR}/googletest" googletest-distribution)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main)
enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
# ---------------------------------------------------