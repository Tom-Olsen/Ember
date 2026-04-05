# Ember/engine/libs/assetLoader/CMakeLists.txt



# ------------------- Project Setup ------------------
set(TARGET_NAME EmberAssetLoader)
set(PROJECT_TYPE STATIC)    # Choose SHARED (.dll/.so) or STATIC (.lib/.a) library (currently only static supported):

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)   # engine
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)           # libs

if(TARGET ${TARGET_NAME})
    return()
endif()
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
# EmberMath:
add_subdirectory("${ENGINE_LIBS_DIR}/math" EmberMath)
target_link_libraries(${TARGET_NAME} PUBLIC EmberMath)

# EmberLogger:
add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
target_link_libraries(${TARGET_NAME} PUBLIC EmberLogger)
# ---------------------------------------------------