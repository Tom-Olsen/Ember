# Ember/engine/libs/entityComponentSystem/CMakeLists.txt



# ------------------- Project Setup ------------------
# Choose SHARED (.dll) or STATIC (.lib) library (currently only static supported):
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name:
cmake_minimum_required(VERSION 3.30)
project(EmberEntityComponentSystem)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # needed to link static libraries into this dynamic library.

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)   # engine
get_filename_component(ENGINE_CORE_DIR "${ENGINE_DIR}/core" ABSOLUTE)           # core
get_filename_component(EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)              # extern
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)           # libs

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
file(GLOB BASIC_COMPONENTS_FILES "src/basicComponents/*.*")
source_group("Basic Components" FILES ${BASIC_COMPONENTS_FILES})

file(GLOB ECS_CLASSES_FILES "src/ecsClasses/*.*")
source_group("ECS Classes" FILES ${ECS_CLASSES_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
add_library(${PROJECT_NAME} STATIC
    ${BASIC_COMPONENTS_FILES}
    ${ECS_CLASSES_FILES})

# Source subdirectories:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/basicComponents
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ecsClasses)
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberCore:
if (NOT TARGET EmberCore)
    add_subdirectory(${ENGINE_CORE_DIR} EmberCore)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberCore)

# EmberLogger:
if (NOT TARGET EmberLogger)
    set(EMBER_LOGGER_BUILD_SHARED ON CACHE BOOL "Build EmberLogger as shared lib." FORCE)
    add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberLogger)

# EmberMath:
if (NOT TARGET EmberMath)
    add_subdirectory("${ENGINE_LIBS_DIR}/math" EmberMath)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EmberMath)
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# EnTT:
set(ENTT_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
if (NOT TARGET EnTT::EnTT)
    add_subdirectory("${EXTERN_DIR}/EnTT" "${CMAKE_BINARY_DIR}/EnTT_build")
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
# ---------------------------------------------------