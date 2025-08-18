# Ember/engine/shaders/CMakeLists.txt



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
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB CPP_HLSL_HEADERS "src/*.h")
file(GLOB VERTEX_SHADERS "src/*.vert.hlsl")
file(GLOB FRAGMENT_SHADERS "src/*.frag.hlsl")
file(GLOB COMPUTE_SHADERS "src/*.comp.hlsl")
file(GLOB SHADER_INCLUDES "src/*.hlsli")
file(GLOB SHADER_SOURCES 
    ${VERTEX_SHADERS}
    ${FRAGMENT_SHADERS}
    ${COMPUTE_SHADERS})
source_group("C++ & HLSL Headers" FILES ${CPP_HLSL_HEADERS})
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
        COMMAND ${DXC_PATH} -spirv -T ${SHADER_TYPE} -E main ${SHADER_FILE} -Fo ${OUTPUT_FILE}
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling shader: ${SHADER_FILE}"
        VERBATIM
    )
    message(STATUS ${SHADER_FILE})
    message(STATUS ${OUTPUT_FILE})
endforeach()

# Shader compile target:
add_custom_target(ShaderCompiler ALL DEPENDS ${SPIRV_OUTPUTS})

# Shader sources added as dummy targets to make them show up in the IDE:
add_custom_target(Shaders SOURCES ${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${COMPUTE_SHADERS} ${SHADER_INCLUDES} ${CPP_HLSL_HEADERS})
# ---------------------------------------------------