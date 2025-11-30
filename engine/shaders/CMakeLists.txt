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
# Gather shader source files and includes from all subdirectories:
file(GLOB COMPUTE_SHADERS "src/compute/*.*")
    file(GLOB BITONIC_SORT_FLOAT "src/compute/bitonicSortFloat/*.*")
    file(GLOB BITONIC_SORT_FLOAT2 "src/compute/bitonicSortFloat2/*.*")
    file(GLOB BITONIC_SORT_INT "src/compute/bitonicSortInt/*.*")
    file(GLOB BITONIC_SORT_UINT "src/compute/bitonicSortUint/*.*")
    file(GLOB PERMUTATION "src/compute/permutation/*.*")
file(GLOB FRAGMENT_SHADERS "src/fragment/*.*")
file(GLOB SHADER_INCLUDES "src/includes/*.*")
file(GLOB CPP_HLSL_INCLUDES "src/includesCppHlsl/*.*")
file(GLOB VERTEX_SHADERS "src/vertex/*.*")

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
add_custom_target(EmberShaderCompiler ALL DEPENDS ${SPIRV_OUTPUTS})

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