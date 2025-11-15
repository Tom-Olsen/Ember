# Ember/projects/fluidDynamics/shaders/CMakeLists.txt



# ------------------- Project Setup ------------------
# Set vulkan sdk path:
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable not set. Please set it before building.")
endif()
set(VULKAN_SDK_PATH $ENV{VULKAN_SDK})

# Set DXC path depending on platform:
if(WIN32)
    set(DXC_PATH "${VULKAN_SDK_PATH}/bin/dxc.exe")
else()
    set(DXC_PATH "${VULKAN_SDK_PATH}/bin/dxc")
endif()

# Cache directories:
get_filename_component(ENGINE_SHADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../engine/shaders" ABSOLUTE)  # engine shaders
# ---------------------------------------------------



# ------------------- File Lists --------------------
# Gather shader source files and includes from all subdirectories:
file(GLOB COMPUTE_SHADERS "src/*.comp.hlsl")
file(GLOB FRAGMENT_SHADERS "src/*.frag.hlsl")
file(GLOB VERTEX_SHADERS "src/*.vert.hlsl")
file(GLOB SHADER_INCLUDES "src/*.hlsli" "${ENGINE_SHADERS_DIR}/src/includes/*.*" "${ENGINE_SHADERS_DIR}/src/includesCppHlsl/*.*")

# Combine shader source files:
file(GLOB SHADER_SOURCES 
    ${COMPUTE_SHADERS}
    ${FRAGMENT_SHADERS}
    ${VERTEX_SHADERS})
    
# Organize files into IDE groups:
source_group("Vertex Shaders" FILES ${VERTEX_SHADERS})
source_group("Fragment Shaders" FILES ${FRAGMENT_SHADERS})
source_group("Compute Shaders" FILES ${COMPUTE_SHADERS})
source_group("Includes" FILES ${SHADER_INCLUDES})
# ---------------------------------------------------



# ----------------- Create Targets ------------------
set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(SPIRV_OUTPUTS "")
foreach(SHADER_FILE ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER_FILE} NAME_WE)
    get_filename_component(EXTENSION ${SHADER_FILE} EXT)
    string(REPLACE ".hlsl" "" EXT_TRIMMED ${EXTENSION}) # trim ".hlsl"
    
    # Shadertype is determined by file extension, not by folder placement:
    if(EXTENSION MATCHES "vert.hlsl$")
        set(SHADER_TYPE "vs_6_0")
    elseif(EXTENSION MATCHES "frag.hlsl$")
        set(SHADER_TYPE "ps_6_0")
    elseif(EXTENSION MATCHES "comp.hlsl$")
        set(SHADER_TYPE "cs_6_0")
    else()
        message(FATAL_ERROR "Unknown shader extension for ${SHADER_FILE}")
    endif()

    set(OUTPUT_FILE "${OUTPUT_DIR}/${FILE_NAME}${EXT_TRIMMED}.spv")
    list(APPEND SPIRV_OUTPUTS ${OUTPUT_FILE})

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        BYPRODUCTS ${OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
        COMMAND ${DXC_PATH} -spirv -T ${SHADER_TYPE} -E main -I ${CMAKE_CURRENT_LIST_DIR}/src -I ${ENGINE_SHADERS_DIR}/src/includes -I ${ENGINE_SHADERS_DIR}/src/includesCppHlsl ${SHADER_FILE} -Fo ${OUTPUT_FILE}
        DEPENDS ${SHADER_FILE} ${SHADER_INCLUDES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMENT "Compiling shader: ${SHADER_FILE}"
        VERBATIM
    )
    message(STATUS ${SHADER_FILE})
    message(STATUS ${OUTPUT_FILE})
endforeach()

# Shader compile target:
add_custom_target(ProjectShaderCompiler ALL DEPENDS ${SPIRV_OUTPUTS})

# Shader sources added as dummy targets to make them show up in the IDE:
add_custom_target(ProjectShaders SOURCES
    ${COMPUTE_SHADERS}
    ${FRAGMENT_SHADERS}
    ${SHADER_INCLUDES}
    ${VERTEX_SHADERS})
# ---------------------------------------------------