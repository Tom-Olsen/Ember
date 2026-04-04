# Ember/engine/libs/entityComponentSystem/CMakeLists.txt



# ------------------- Project Setup ------------------
set(TARGET_NAME EmberEntityComponentSystem)
set(PROJECT_TYPE STATIC)    # Choose SHARED (.dll/.so) or STATIC (.lib/.a) library (currently only static supported):

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)   # engine
get_filename_component(ENGINE_CORE_DIR "${ENGINE_DIR}/core" ABSOLUTE)           # core
get_filename_component(EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)              # extern
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)           # libs
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB BASIC_COMPONENTS_FILES "src/basicComponents/*.*")
source_group("Basic Components" FILES ${BASIC_COMPONENTS_FILES})

file(GLOB ECS_CLASSES_FILES "src/ecsClasses/*.*")
source_group("ECS Classes" FILES ${ECS_CLASSES_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${TARGET_NAME} STATIC
    ${BASIC_COMPONENTS_FILES}
    ${ECS_CLASSES_FILES})

# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/basicComponents
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ecsClasses)
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberCore:
add_subdirectory(${ENGINE_CORE_DIR} EmberCore)
target_link_libraries(${TARGET_NAME} PUBLIC EmberCore)

# EmberLogger:
set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib." FORCE)
add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
target_link_libraries(${TARGET_NAME} PUBLIC EmberLogger)

# EmberMath:
add_subdirectory("${ENGINE_LIBS_DIR}/math" EmberMath)
target_link_libraries(${TARGET_NAME} PUBLIC EmberMath)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# EnTT:
set(ENTT_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${EXTERN_DIR}/EnTT" "${CMAKE_BINARY_DIR}/EnTT_build")
target_link_libraries(${TARGET_NAME} PUBLIC EnTT::EnTT)
# ---------------------------------------------------