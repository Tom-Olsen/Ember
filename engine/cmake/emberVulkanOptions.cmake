include_guard(GLOBAL)

option(EMBER_ENABLE_VULKAN_VALIDATION_LAYERS "Enable Vulkan validation layers and related debug helpers." OFF)
message(STATUS "EMBER_ENABLE_VULKAN_VALIDATION_LAYERS: ${EMBER_ENABLE_VULKAN_VALIDATION_LAYERS}")

function(ember_configure_vulkan_validation_layers targetName)
    if(EMBER_ENABLE_VULKAN_VALIDATION_LAYERS)
        target_compile_definitions(${targetName} PRIVATE VALIDATION_LAYERS_ACTIVE)
    endif()
endfunction()