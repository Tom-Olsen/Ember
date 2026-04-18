# Ember/applications/emberHeadlessApp/CMakeLists.txt



# ------------------- Project Setup ------------------
set(TARGET_NAME EmberHeadlessApp)
set(PROJECT_TYPE STATIC)    # Choose SHARED (.dll/.so) or STATIC (.lib/.a) library (currently only static supported):

# Cache directories:
get_filename_component(ENGINE_APPLICATIONS_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)    # application
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)           # engine
get_filename_component(ENGINE_BACKENDS_DIR "${ENGINE_DIR}/backends" ABSOLUTE)           # backends
get_filename_component(ENGINE_CORE_DIR "${ENGINE_DIR}/core" ABSOLUTE)                   # core
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)                   # libs

if(TARGET ${TARGET_NAME})
    return()
endif()
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB SOURCE_FILES "${ENGINE_APPLICATIONS_DIR}/src/*.*")
source_group("Source Files" FILES ${SOURCE_FILES})
# ---------------------------------------------------
 
 
 
# ----------------- Create Library ------------------
add_library(${TARGET_NAME} STATIC
    ${SOURCE_FILES})
 
# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC
    ${ENGINE_APPLICATIONS_DIR}/src)
# ---------------------------------------------------
 
 
 
# ---------------------- Macros ---------------------
target_compile_definitions(${TARGET_NAME} PUBLIC ENGINE_APPLICATIONS_DIR="${ENGINE_APPLICATIONS_DIR}")
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberCore:
add_subdirectory("${ENGINE_CORE_DIR}" EmberCore)
target_link_libraries(${TARGET_NAME} PUBLIC EmberCore)

# EmberEntityComponentSystem:
add_subdirectory("${ENGINE_LIBS_DIR}/entityComponentSystem" EmberEntityComponentSystem)
target_link_libraries(${TARGET_NAME} PUBLIC EmberEntityComponentSystem)
# ---------------------------------------------------



# ------------------ Link Backends ------------------
# Vulkan Renderer:
add_subdirectory("${ENGINE_BACKENDS_DIR}/vulkanRenderer" VulkanRendererBackend)
target_link_libraries(${TARGET_NAME} PUBLIC VulkanRendererBackend)

# Null Window:
add_subdirectory("${ENGINE_BACKENDS_DIR}/nullWindow" NullWindowBackend)
target_link_libraries(${TARGET_NAME} PUBLIC NullWindowBackend)

# Null Gui:
add_subdirectory("${ENGINE_BACKENDS_DIR}/nullGui" NullGuiBackend)
target_link_libraries(${TARGET_NAME} PUBLIC NullGuiBackend)
# ---------------------------------------------------