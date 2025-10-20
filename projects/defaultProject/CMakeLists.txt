# Ember/defaultProject/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.30)
project(DefaultProject)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
# src/main.cpp:
file(GLOB MAIN_FILE "src/main.cpp")
source_group("/" FILES ${MAIN_FILE})

# src/scenes/*:
file(GLOB SCENE_FILES "src/scenes/*")
source_group("Scenes" FILES ${SCENE_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE} ${SCENE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/scenes)

# Add project root path as a macro:
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# ------------------ Link Libraries -----------------
## Ember Game Debug Application:
#add_subdirectory("../../applications/emberGameDebugApp" EmberGameDebugApp)
#target_link_libraries(${PROJECT_NAME} PUBLIC EmberGameDebugApp)

# Ember Editor Application:
add_subdirectory("../../applications/emberEditorApp" EmberEditorApp)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberEditorApp)
# ---------------------------------------------------