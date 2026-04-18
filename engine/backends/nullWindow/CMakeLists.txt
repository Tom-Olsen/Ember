# Ember/engine/backends/nullWindow/CMakeLists.txt



# ------------------- Project Setup ------------------
# Static (default) vs Shared library option:
set(TARGET_NAME NullWindowBackend)
option(NULL_WINDOW_BACKEND_BUILD_SHARED "Build ${TARGET_NAME} as a shared library" ON)

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)       # engine
get_filename_component(ENGINE_COMMON_DIR "${ENGINE_DIR}/common" ABSOLUTE)           # commom
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)           # extern
get_filename_component(ENGINE_INTERFACES_DIR "${ENGINE_DIR}/interfaces" ABSOLUTE)   # interfaces
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)               # libs

if(TARGET ${TARGET_NAME})
    return()
endif()
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB SOURCE_FILES "src/*.*")
source_group("Source Files" FILES ${SOURCE_FILES})

file(GLOB COMMON_FILES "${ENGINE_COMMON_DIR}/*.*")
source_group("Commen" FILES ${COMMON_FILES})

file(GLOB WINDOW_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/window/*.*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
set(LIB_TYPE STATIC)
if(NULL_WINDOW_BACKEND_BUILD_SHARED)
    set(LIB_TYPE SHARED)
endif()
add_library(${TARGET_NAME} ${LIB_TYPE}
    ${SOURCE_FILES}
    ${COMMON_FILES}
    ${WINDOW_INTERFACE_FILES})

# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${ENGINE_COMMON_DIR}
    ${ENGINE_INTERFACES_DIR}/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
if(NULL_WINDOW_BACKEND_BUILD_SHARED)
    target_compile_definitions(${TARGET_NAME} PRIVATE NULL_WINDOW_BACKEND_EXPORTS)
else()
    target_compile_definitions(${TARGET_NAME} PUBLIC NULL_WINDOW_BACKEND_STATIC)
endif()
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberLogger:
add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
target_link_libraries(${TARGET_NAME} PUBLIC EmberLogger)

# EmberMath:
 add_subdirectory("${ENGINE_LIBS_DIR}/math" EmberMath)
target_link_libraries(${TARGET_NAME} PUBLIC EmberMath)

# EmberTaskSystem:
add_subdirectory("${ENGINE_LIBS_DIR}/taskSystem" EmberTaskSystem)
target_link_libraries(${TARGET_NAME} PUBLIC EmberTaskSystem)
# ---------------------------------------------------