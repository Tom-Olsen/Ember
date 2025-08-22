# Ember/engine/core/gameMode/CMakeLists.txt

# Include the core build function:
include("${CMAKE_CURRENT_LIST_DIR}/../coreBuild.cmake")

# Build game mode variant:
# Function arguments: target DEAR_IMGUI_ENABLED EDITOR_ENABLED WINDOW_ENABLED
build_ember_core(OFF OFF ON)