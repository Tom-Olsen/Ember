# Ember/engine/libs/spirvReflect/CMakeLists.txt



# ------------------- Project Setup ------------------
set(PROJECT_TYPE STATIC)    # Choose SHARED (.dll/.so) or STATIC (.lib/.a) library (currently only static supported):
set(TARGET_NAME EmberSpirvReflect)


# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)               # engine
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)                       # libs
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)                   # extern
get_filename_component(ENGINE_VULKAN_UTILITY_DIR "${ENGINE_DIR}/vulkanUtility" ABSOLUTE)    # vulkan utility
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB SRC_FILES "src/*")
source_group("Source Files" FILES ${SRC_FILES})

file(GLOB VULKAN_UTILITY_FILES "${ENGINE_VULKAN_UTILITY_DIR}/*")
source_group("Vulkan Utility" FILES ${VULKAN_UTILITY_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${TARGET_NAME} ${PROJECT_TYPE} ${SRC_FILES} ${VULKAN_UTILITY_FILES})

# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC 
    ${ENGINE_VULKAN_UTILITY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberBufferLayout:
add_subdirectory("${ENGINE_LIBS_DIR}/bufferLayout" EmberBufferLayout)
target_link_libraries(${TARGET_NAME} PUBLIC EmberBufferLayout)

# EmberLogger:
set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib.")
add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
target_link_libraries(${TARGET_NAME} PUBLIC EmberLogger)

# EmberMath:
add_subdirectory("${ENGINE_LIBS_DIR}/math" EmberMath)
target_link_libraries(${TARGET_NAME} PUBLIC EmberMath)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# Vulkan (dynamicly linked):
find_package(Vulkan REQUIRED)   # required to be installed
target_include_directories(${TARGET_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC ${Vulkan_LIBRARIES})

# SPIRV-Reflect:
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE)  # enable static lib compilation for SPIRV-Reflect
add_subdirectory("${ENGINE_EXTERN_DIR}/SPIRV-Reflect" "${CMAKE_BINARY_DIR}/SPIRV-Reflect_build")
target_link_libraries(${TARGET_NAME} PUBLIC spirv-reflect-static)
# ---------------------------------------------------