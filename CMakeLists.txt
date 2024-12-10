# Engine/CMakeLists.txt

# Choose SHARED (.dll) or STATIC (.lib) library
set(PROJECT_TYPE STATIC)

# Minimum version requirement and project name
cmake_minimum_required(VERSION 3.31)
project(MyEngine)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")


   
# ------------------- File lists --------------------
# shaders/*:
file(GLOB SHADER_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsl"
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsli"
    "${PROJECT_SOURCE_DIR}/shaders/*.bat")
source_group("Shaders" FILES ${SHADER_FILES})

# src/gameObjectSystem/*:
file(GLOB GAME_OBJECT_SYSTEM_FILES "${PROJECT_SOURCE_DIR}/src/gameObjectSystem/*")
source_group("Game Object System" FILES ${GAME_OBJECT_SYSTEM_FILES})

# src/managers/*:
file(GLOB MANAGERS_FILES "${PROJECT_SOURCE_DIR}/src/managers/*")
source_group("Managers" FILES ${MANAGERS_FILES})

# src/mathf/*:
file(GLOB MATHF_FILES "${PROJECT_SOURCE_DIR}/src/mathf/*")
source_group("Mathf" FILES ${MATHF_FILES})

# src/renderPassesAndPipelines/*:
file(GLOB RENDERPASSES_AND_PIPELINES_FILES "${PROJECT_SOURCE_DIR}/src/renderPassesAndPipelines/*")
source_group("Render Passes & Pipelines" FILES ${RENDERPASSES_AND_PIPELINES_FILES})

# src/renderResources/*:
file(GLOB RENDER_RESOURCES_FILES "${PROJECT_SOURCE_DIR}/src/renderResources/*")
source_group("Render Resources" FILES ${RENDER_RESOURCES_FILES})

# src/utility/*:
file(GLOB UTILITY_FILES "${PROJECT_SOURCE_DIR}/src/utility/*")
source_group("Utility" FILES ${UTILITY_FILES})

# src/vulkanRenderer/*:
file(GLOB VULKAN_RENDERER_FILES "${PROJECT_SOURCE_DIR}/src/vulkanRenderer/*")
source_group("VulkanRenderer" FILES ${VULKAN_RENDERER_FILES})

# src/*
file(GLOB OTHER_FILES "${PROJECT_SOURCE_DIR}/src/*")
source_group("Other" FILES ${OTHER_FILES})
# ---------------------------------------------------



# Configure executables:
add_executable(${PROJECT_NAME}
${SHADER_FILES}
${GAME_OBJECT_SYSTEM_FILES}
${MANAGERS_FILES}
${MATHF_FILES}
${RENDERPASSES_AND_PIPELINES_FILES}
${RENDER_RESOURCES_FILES}
${UTILITY_FILES}
${VULKAN_RENDERER_FILES}
${OTHER_FILES})
add_dependencies(${PROJECT_NAME} build_shaders)



# ---------------- Shader Compilation ---------------
set(SHADER_LANGUAGE "HLSL") # Set shader Language (HLSL or GLSL):

# Mark shader files as header-only to prevent compilation
foreach(SHADER_FILE ${SHADER_FILES})
    set_source_files_properties(${SHADER_FILE} PROPERTIES HEADER_FILE_ONLY ON)
endforeach()

# Unix compilation script:
if(UNIX)
    add_custom_target(build_shaders ALL
        COMMAND "{PROJECT_SOURCE_DIR}/shaders/compile${SHADER_LANGUAGE}.sh"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
        BYPRODUCTS ${PROJECT_SOURCE_DIR}/shaders/*.spv)
endif(UNIX)

# Windows compilation script:
if(WIN32)
    add_custom_target(build_shaders ALL
        COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile${SHADER_LANGUAGE}.bat"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
        BYPRODUCTS ${PROJECT_SOURCE_DIR}/shaders/*.spv)
endif(WIN32)
# ---------------------------------------------------



# -------------------- Libraries --------------------
# SDL (cMakeList target):
add_subdirectory(libs/SDL)
target_include_directories(${PROJECT_NAME} PUBLIC libs/SDL/include)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3-shared)

# spdlog (cMakeList target):
add_subdirectory(libs/spdlog)
target_include_directories(${PROJECT_NAME} PUBLIC libs/spdlog/include)

# SPIRV-Reflect (cMakeList target):
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "Build static library for SPIRV-Reflect" FORCE) # enable static lib compilation for SPIRV-Reflect
add_subdirectory(libs/SPIRV-Reflect)
target_include_directories(${PROJECT_NAME} PUBLIC libs/SPIRV-Reflect)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-reflect-static)

# Vulkan (dynamicly linked):
find_package(Vulkan REQUIRED)           # required to be installed
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})

# Vulkan Memory Allocator: (header only)
target_include_directories(${PROJECT_NAME} PUBLIC libs/vma/include)

# Source subdirectories:
target_include_directories(${PROJECT_NAME}
PRIVATE ${CMAKE_SOURCE_DIR}/src/gameobjectSystem
PRIVATE ${CMAKE_SOURCE_DIR}/src/managers
PRIVATE ${CMAKE_SOURCE_DIR}/src/mathf
PRIVATE ${CMAKE_SOURCE_DIR}/src/renderPassesAndPipelines
PRIVATE ${CMAKE_SOURCE_DIR}/src/renderResources
PRIVATE ${CMAKE_SOURCE_DIR}/src/utility
PRIVATE ${CMAKE_SOURCE_DIR}/src/vulkanRenderer
PRIVATE ${CMAKE_SOURCE_DIR}/src)
# ---------------------------------------------------