# Ember/engine/libs/vulkanUtility/CMakeLists.txt



# ------------------- Project Setup ------------------
set(TARGET_NAME EmberVulkanUtility)
set(PROJECT_TYPE STATIC)    # Choose SHARED (.dll/.so) or STATIC (.lib/.a) library (currently only static supported):

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

# Allow linking this static library into shared libraries:
set_target_properties(${TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# Vulkan (dynamicly linked):
find_package(Vulkan REQUIRED)   # required to be installed
target_include_directories(${TARGET_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC ${Vulkan_LIBRARIES})
# ---------------------------------------------------