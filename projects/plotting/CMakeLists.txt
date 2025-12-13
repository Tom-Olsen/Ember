## Ember/plotting/CMakeLists.txt
#
# This project currently makes no sense
#
## ------------------ Project Setup ------------------
#cmake_minimum_required(VERSION 3.30)
#project(Plotting)
#
## C++ standard:
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
## Cache directories:
#get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine" ABSOLUTE)    # engine
#
## Output directory:
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
## ---------------------------------------------------
#
#
#
## ------------------- File Lists --------------------
## src/*:
#file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*")
#source_group("Source Files" FILES ${SOURCE_FILES})
## ---------------------------------------------------
#
#
#
## ---------------- Create Executable ----------------
#add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES})
#
## Add project root path as a macro:
#get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
#target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
#
## Add src subfolders as include directories:
#target_include_directories(${PROJECT_NAME}
#PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
## ---------------------------------------------------
#
#
#
## ------------------ Link Libraries -----------------
## Ember:
#add_subdirectory("../Engine" EmberEngine)
#target_link_libraries(${PROJECT_NAME} PUBLIC EmberEngine)
#
## Shaders:
#add_subdirectory("src/shaders")
#add_dependencies(${PROJECT_NAME} CompilePlottingShaders)
## ---------------------------------------------------