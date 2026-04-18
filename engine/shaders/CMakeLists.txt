# Ember/engine/shaders/CMakeLists.txt



# ------------------- Project Setup ------------------
set(TARGET_NAME EmberShaderCompiler)

# Cache directories:
get_filename_component(COMPUTE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/compute" ABSOLUTE)                    # compute shaders  (.comp.hlsl)
get_filename_component(FRAGMENT_DIR "${CMAKE_CURRENT_LIST_DIR}/src/fragment" ABSOLUTE)                  # fragment shaders (.frag.hlsl)
get_filename_component(INCLUDES_DIR "${CMAKE_CURRENT_LIST_DIR}/src/includes" ABSOLUTE)                  # shader includes  (.hlsli)
get_filename_component(INCLUDES_CPP_HLSL_DIR "${CMAKE_CURRENT_LIST_DIR}/src/includesCppHlsl" ABSOLUTE)  # shader+cpp includes  (.h)
get_filename_component(VERTEX_DIR "${CMAKE_CURRENT_LIST_DIR}/src/vertex" ABSOLUTE)                      # vertex includes  (.vert.hlsl)

# Set vulkan sdk path:
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable not set. Please set it before building.")
endif()
set(VULKAN_SDK_PATH $ENV{VULKAN_SDK})

# Set DXC path depending on platform:
if(WIN32)
    set(DXC_PATH "${VULKAN_SDK_PATH}/bin/dxc.exe")
else()
    find_program(DXC_PATH dxc REQUIRED)
endif()

if(TARGET ${TARGET_NAME})
    return()
endif()
# ---------------------------------------------------



# ------------------- File Lists --------------------
# Gather shader source files and includes from all subdirectories:
file(GLOB COMPUTE_SHADERS "${COMPUTE_DIR}/*.*")
    file(GLOB BITONIC_SORT_FLOAT "${COMPUTE_DIR}/bitonicSortFloat/*.*")
    file(GLOB BITONIC_SORT_FLOAT2 "${COMPUTE_DIR}/bitonicSortFloat2/*.*")
    file(GLOB BITONIC_SORT_INT "${COMPUTE_DIR}/bitonicSortInt/*.*")
    file(GLOB BITONIC_SORT_UINT "${COMPUTE_DIR}/bitonicSortUint/*.*")
    file(GLOB PERMUTATION "${COMPUTE_DIR}/permutation/*.*")
file(GLOB FRAGMENT_SHADERS "${FRAGMENT_DIR}/*.*")
file(GLOB SHADER_INCLUDES "${INCLUDES_DIR}/*.*")
file(GLOB CPP_HLSL_INCLUDES "${INCLUDES_CPP_HLSL_DIR}/*.*")
file(GLOB VERTEX_SHADERS "${VERTEX_DIR}/*.*")

# Combine shader source files:
file(GLOB SHADER_SOURCES 
    ${COMPUTE_SHADERS}
    ${BITONIC_SORT_FLOAT}
    ${BITONIC_SORT_FLOAT2}
    ${BITONIC_SORT_INT}
    ${BITONIC_SORT_UINT}
    ${PERMUTATION}
    ${FRAGMENT_SHADERS}
    ${VERTEX_SHADERS})

# Organize files into IDE groups:
source_group("Compute Shaders" FILES ${COMPUTE_SHADERS})
source_group("Compute Shaders/Bitonic Sort Float" FILES ${BITONIC_SORT_FLOAT})
source_group("Compute Shaders/Bitonic Sort Float2" FILES ${BITONIC_SORT_FLOAT2})
source_group("Compute Shaders/Bitonic Sort Int" FILES ${BITONIC_SORT_INT})
source_group("Compute Shaders/Bitonic Sort Uint" FILES ${BITONIC_SORT_UINT})
source_group("Compute Shaders/Permutation" FILES ${PERMUTATION})
source_group("Fragment Shaders" FILES ${FRAGMENT_SHADERS})
source_group("Includes" FILES ${SHADER_INCLUDES})
source_group("Includes C++ & HLSL" FILES ${CPP_HLSL_INCLUDES})
source_group("Vertex Shaders" FILES ${VERTEX_SHADERS})
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
        COMMAND ${DXC_PATH} -spirv -T ${SHADER_TYPE} -E main -I ${CMAKE_CURRENT_LIST_DIR}/src/includes -I ${CMAKE_CURRENT_LIST_DIR}/src/includesCppHlsl ${SHADER_FILE} -Fo ${OUTPUT_FILE}
        DEPENDS ${SHADER_FILE} ${SHADER_INCLUDES} ${CPP_HLSL_INCLUDES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMENT "Compiling shader: ${SHADER_FILE}"
        VERBATIM
    )
    message(STATUS ${SHADER_FILE})
    message(STATUS ${OUTPUT_FILE})
endforeach()

# Shader compile target:
add_custom_target(${TARGET_NAME} ALL DEPENDS ${SPIRV_OUTPUTS})

# Shader sources added as dummy targets to make them show up in the IDE:
add_custom_target(EmberShaders SOURCES
    ${COMPUTE_SHADERS}
    ${BITONIC_SORT_FLOAT}
    ${BITONIC_SORT_FLOAT2}
    ${BITONIC_SORT_INT}
    ${BITONIC_SORT_UINT}
    ${PERMUTATION}
    ${FRAGMENT_SHADERS}
    ${SHADER_INCLUDES}
    ${CPP_HLSL_INCLUDES}
    ${VERTEX_SHADERS})
# ---------------------------------------------------