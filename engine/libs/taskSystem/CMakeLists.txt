# Ember/engine/libs/taskSystem/CMakeLists.txt



# ------------------- Project Setup ------------------
set(TARGET_NAME EmberTaskSystem)
set(PROJECT_TYPE STATIC)    # Choose SHARED (.dll/.so) or STATIC (.lib/.a) library (currently only static supported):

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)   # engine
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)       # extern
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB SRC_FILES "src/*")
source_group("Source Files" FILES ${SRC_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${TARGET_NAME} ${PROJECT_TYPE} ${SRC_FILES})

# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberLogger:
set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib.")
add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
target_link_libraries(${TARGET_NAME} PUBLIC EmberLogger)
# ---------------------------------------------------