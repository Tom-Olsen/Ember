# Ember/engine/backends/sdlWindow/CMakeLists.txt



# ------------------- Project Setup ------------------
# Static (default) vs Shared library option:
set(TARGET_NAME SdlWindowBackend)
option(SDL_WINDOW_BACKEND_BUILD_SHARED "Build ${TARGET_NAME} as a shared library" ON)

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

file(GLOB GUI_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/gui/*.*")
source_group("Gui Interface" FILES ${GUI_INTERFACE_FILES})

file(GLOB RENDERER_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/renderer/*.*")
source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

file(GLOB WINDOW_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/window/*.*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
set(LIB_TYPE STATIC)
if(SDL_WINDOW_BACKEND_BUILD_SHARED)
    set(LIB_TYPE SHARED)
endif()
add_library(${TARGET_NAME} ${LIB_TYPE}
    ${SOURCE_FILES}
    ${COMMON_FILES}
    ${GUI_INTERFACE_FILES}
    ${RENDERER_INTERFACE_FILES}
    ${WINDOW_INTERFACE_FILES})

# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${ENGINE_COMMON_DIR}
    ${ENGINE_INTERFACES_DIR}/gui
    ${ENGINE_INTERFACES_DIR}/renderer
    ${ENGINE_INTERFACES_DIR}/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
if(SDL_WINDOW_BACKEND_BUILD_SHARED)
    target_compile_definitions(${TARGET_NAME} PRIVATE SDL_WINDOW_BACKEND_EXPORTS)
else()
    target_compile_definitions(${TARGET_NAME} PUBLIC SDL_WINDOW_BACKEND_STATIC)
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



# ------------- Link External Libraries -------------
# SDL (CMakeList target):
if (NOT TARGET SDL3::SDL3-shared)
    set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as a static library" FORCE)
    set(SDL_SHARED ON CACHE BOOL "Build SDL3 as a shared library" FORCE)
    add_subdirectory("${ENGINE_EXTERN_DIR}/SDL" "${CMAKE_BINARY_DIR}/SDL_build")
endif()
target_link_libraries(${TARGET_NAME} PUBLIC SDL3::SDL3-shared)

# Vulkan (dynamically linked):
find_package(Vulkan REQUIRED)           # required to be installed
target_include_directories(${TARGET_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC ${Vulkan_LIBRARIES})
# ---------------------------------------------------