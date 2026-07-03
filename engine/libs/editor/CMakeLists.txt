# Ember/engine/libs/editor/CMakeLists.txt



# ------------------- Project Setup ------------------
set(TARGET_NAME EmberEditor)
set(PROJECT_TYPE STATIC)    # Choose SHARED (.dll/.so) or STATIC (.lib/.a) library (currently only static supported):

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)   # engine
get_filename_component(ENGINE_CORE_DIR "${ENGINE_DIR}/core" ABSOLUTE)           # core
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)           # libs

if(TARGET ${TARGET_NAME})
    return()
endif()
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB SRC_FILES "src/*.*")
source_group("Source Files" FILES ${SRC_FILES})

file(GLOB HANDLES_FILES "src/handles/*.*")
source_group("Handles" FILES ${HANDLES_FILES})

file(GLOB EDITOR_WINDOWS_FILES "src/editorWindows/*.*")
source_group("Editor Windows" FILES ${EDITOR_WINDOWS_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${TARGET_NAME} STATIC
    ${SRC_FILES}
    ${HANDLES_FILES}
    ${EDITOR_WINDOWS_FILES})

# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/editorWindows
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handles)

# Allow linking this static library into shared libraries:
set_target_properties(${TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberCore:
add_subdirectory("${ENGINE_CORE_DIR}" EmberCore)
target_link_libraries(${TARGET_NAME} PUBLIC EmberCore)

# EmberEntityComponentSystem:
add_subdirectory("${ENGINE_LIBS_DIR}/entityComponentSystem" EmberEntityComponentSystem)
target_link_libraries(${TARGET_NAME} PUBLIC EmberEntityComponentSystem)
# ---------------------------------------------------