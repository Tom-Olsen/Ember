# Ember/engine/core/CMakeLists.txt



# ------------------- Project Setup ------------------
# Static (default) vs Shared library option:
set(TARGET_NAME EmberCore)
option(EMBER_CORE_BUILD_SHARED "Build ${TARGET_NAME} as a shared library" ON)

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)          # engine
get_filename_component(ENGINE_CORE_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)        # core
get_filename_component(ENGINE_COMMON_DIR "${ENGINE_DIR}/common" ABSOLUTE)           # commom
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)           # extern
get_filename_component(ENGINE_INTERFACES_DIR "${ENGINE_DIR}/interfaces" ABSOLUTE)   # interfaces
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)               # libs
get_filename_component(ENGINE_RESOURCES_DIR "${ENGINE_DIR}/resources" ABSOLUTE)     # resources
get_filename_component(ENGINE_SHADERS_DIR "${ENGINE_DIR}/shaders" ABSOLUTE)         # shaders

if(TARGET ${TARGET_NAME})
    return()
endif()
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB ENGINE_FILE "${ENGINE_CORE_DIR}/src/*.*")
source_group("/" FILES ${ENGINE_FILE})
 
file(GLOB EDITOR_FILES "${ENGINE_CORE_DIR}/src/editor/*.*")
source_group("Editor" FILES ${EDITOR_FILES})

file(GLOB EVENT_SYSTEM_FILES "${ENGINE_CORE_DIR}/src/eventSystem/*.*")
source_group("Event System" FILES ${EVENT_SYSTEM_FILES})

file(GLOB GPU_RESOURCES_FILES "${ENGINE_CORE_DIR}/src/gpuResources/*.*")
source_group("Gpu Resources" FILES ${GPU_RESOURCES_FILES})
 
file(GLOB GUI_FILES "${ENGINE_CORE_DIR}/src/gui/*.*")
source_group("Gui" FILES ${GUI_FILES})
 
file(GLOB MANAGERS_FILES "${ENGINE_CORE_DIR}/src/managers/*.*")
source_group("Managers" FILES ${MANAGERS_FILES})
 
file(GLOB PHYSICS_FILES "${ENGINE_CORE_DIR}/src/physics/*.*")
source_group("Physics" FILES ${PHYSICS_FILES})
 
file(GLOB RENDERER_FILES "${ENGINE_CORE_DIR}/src/renderer/*.*")
source_group("Renderer" FILES ${RENDERER_FILES})
 
file(GLOB UTILITY_FILES "${ENGINE_CORE_DIR}/src/utility/*.*")
source_group("Utility" FILES ${UTILITY_FILES})

file(GLOB WINDOW_FILES "${ENGINE_CORE_DIR}/src/window/*.*")
source_group("Window" FILES ${WINDOW_FILES})

file(GLOB COMMON_FILES "${ENGINE_COMMON_DIR}/*.*")
source_group("Common" FILES ${COMMON_FILES})

file(GLOB GUI_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/gui/*.*")
source_group("Gui Interface" FILES ${GUI_INTERFACE_FILES})
 
file(GLOB RENDERER_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/renderer/*.*")
source_group("Renderer Interface" FILES ${RENDERER_INTERFACE_FILES})

file(GLOB WINDOW_INTERFACE_FILES "${ENGINE_INTERFACES_DIR}/window/*.*")
source_group("Window Interface" FILES ${WINDOW_INTERFACE_FILES})
# ---------------------------------------------------
 
 
 
# ----------------- Create Library ------------------
set(LIB_TYPE STATIC)
if(VULKAN_RENDERER_BACKEND_BUILD_SHARED)
    set(LIB_TYPE SHARED)
endif()
add_library(${TARGET_NAME} ${LIB_TYPE}
    ${ENGINE_FILE}
    ${EDITOR_FILES}
    ${EVENT_SYSTEM_FILES}
    ${GPU_RESOURCES_FILES}
    ${GUI_FILES}
    ${MANAGERS_FILES}
    ${PHYSICS_FILES}
    ${RENDERER_FILES}
    ${UTILITY_FILES}
    ${WINDOW_FILES}
    ${COMMON_FILES}
    ${GUI_INTERFACE_FILES}
    ${RENDERER_INTERFACE_FILES}
    ${WINDOW_INTERFACE_FILES})

# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC
    ${ENGINE_SHADERS_DIR}/src/includesCppHlsl # needed for .h files from shaders, which contian constans for c++ and hlsl, e.g. shadowMap sizes
    ${ENGINE_CORE_DIR}/src
    ${ENGINE_CORE_DIR}/src/editor
    ${ENGINE_CORE_DIR}/src/eventSystem
    ${ENGINE_CORE_DIR}/src/gpuResources
    ${ENGINE_CORE_DIR}/src/gui
    ${ENGINE_CORE_DIR}/src/managers
    ${ENGINE_CORE_DIR}/src/physics
    ${ENGINE_CORE_DIR}/src/renderer
    ${ENGINE_CORE_DIR}/src/utility
    ${ENGINE_CORE_DIR}/src/window
    ${ENGINE_COMMON_DIR}
    ${ENGINE_INTERFACES_DIR}/gui
    ${ENGINE_INTERFACES_DIR}/renderer
    ${ENGINE_INTERFACES_DIR}/window)
# ---------------------------------------------------



# ---------------------- Macros ---------------------
target_compile_definitions(${TARGET_NAME} PUBLIC ENGINE_CORE_DIR="${ENGINE_CORE_DIR}")
target_compile_definitions(${TARGET_NAME} PUBLIC ENGINE_RESOURCES_DIR="${ENGINE_RESOURCES_DIR}")
target_compile_definitions(${TARGET_NAME} PUBLIC ENGINE_SHADERS_DIR="${ENGINE_SHADERS_DIR}")

# Library compilation logic:
if(EMBER_CORE_BUILD_SHARED)
    target_compile_definitions(${TARGET_NAME} PRIVATE EMBER_CORE_EXPORTS)
else()
    target_compile_definitions(${TARGET_NAME} PUBLIC EMBER_CORE_STATIC)
endif()
# ---------------------------------------------------



# -------------- Link Custom Libraries --------------
# EmberAssetLoader:
add_subdirectory("${ENGINE_LIBS_DIR}/assetLoader" EmberAssetLoader)
target_link_libraries(${TARGET_NAME} PUBLIC EmberAssetLoader)

# EmberLogger:
add_subdirectory("${ENGINE_LIBS_DIR}/logger" EmberLogger)
target_link_libraries(${TARGET_NAME} PUBLIC EmberLogger)

# EmberMath:
add_subdirectory("${ENGINE_LIBS_DIR}/math" EmberMath)
target_link_libraries(${TARGET_NAME} PUBLIC EmberMath)

# EmberTaskSystem:
add_subdirectory("${ENGINE_LIBS_DIR}/taskSystem" EmberTaskSystem)
target_link_libraries(${TARGET_NAME} PUBLIC EmberTaskSystem)

# Shaders:
add_subdirectory(${ENGINE_SHADERS_DIR} EmberShaderCompiler)
add_dependencies(${TARGET_NAME} EmberShaderCompiler)
# ---------------------------------------------------