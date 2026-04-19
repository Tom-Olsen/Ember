# Ember/projects/defaultProject/CMakeLists.txt



# ------------------ Project Setup ------------------
cmake_minimum_required(VERSION 3.30)
project(DefaultProject)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../engine" ABSOLUTE)    # engine
get_filename_component(ENGINE_APPLICATIONS_DIR "${ENGINE_DIR}/applications" ABSOLUTE)   # applications

# Output directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # .a .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .so .dylib .dll, must be next to .exe on windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .exe
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB MAIN_FILE "src/main.cpp")
source_group("/" FILES ${MAIN_FILE})

file(GLOB COMPONENTS_FILES "src/components/*.*")
source_group("Components" FILES ${COMPONENTS_FILES})

file(GLOB SCENE_FILES "src/scenes/*.*")
source_group("Scenes" FILES ${SCENE_FILES})
# ---------------------------------------------------



# ---------------- Create Executable ----------------
add_executable(${PROJECT_NAME} ${MAIN_FILE}
	${COMPONENTS_FILES}
	${SCENE_FILES})
    
# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/components
	${CMAKE_CURRENT_SOURCE_DIR}/src/scenes)

# Add project root path as a macro:
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_ROOT_PATH="${PROJECT_ROOT_PATH}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
## Ember Game Application:
#NGINE_APPLICATIONS_DIR}/emberGameApp" EmberGameApp)
#(${PROJECT_NAME} PUBLIC EmberGameApp)

# Ember Editor Application:
add_subdirectory("${ENGINE_APPLICATIONS_DIR}/emberEditorApp" EmberEditorApp)
target_link_libraries(${PROJECT_NAME} PUBLIC EmberEditorApp)
# ---------------------------------------------------