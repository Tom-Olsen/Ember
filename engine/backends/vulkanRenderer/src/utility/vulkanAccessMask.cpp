#include "vulkanAccessMask.h"
#include "vulkanAccessFlags2ToString.h"



// Fallback for Vulkan headers that predate VK_EXT_descriptor_heap; values match the Vulkan spec.
#ifndef VK_EXT_descriptor_heap
static const VkAccessFlagBits2 VK_ACCESS_2_SAMPLER_HEAP_READ_BIT_EXT = 0x200000000000000ULL;
static const VkAccessFlagBits2 VK_ACCESS_2_RESOURCE_HEAP_READ_BIT_EXT = 0x400000000000000ULL;
#endif



namespace vulkanRendererBackend
{
    // VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT:
    AccessMask AccessMasks::TopOfPipe::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_TRANSFER_BIT:
    AccessMask AccessMasks::Transfer::transferRead = VK_ACCESS_2_TRANSFER_READ_BIT;
    AccessMask AccessMasks::Transfer::transferWrite = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    AccessMask AccessMasks::Transfer::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::Transfer::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::Transfer::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT:
    AccessMask AccessMasks::ComputeShader::uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
    AccessMask AccessMasks::ComputeShader::shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
    AccessMask AccessMasks::ComputeShader::shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
    AccessMask AccessMasks::ComputeShader::sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
    AccessMask AccessMasks::ComputeShader::storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    AccessMask AccessMasks::ComputeShader::storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    AccessMask AccessMasks::ComputeShader::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::ComputeShader::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::ComputeShader::accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    AccessMask AccessMasks::ComputeShader::descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
    AccessMask AccessMasks::ComputeShader::shaderBindingTableRead = VK_ACCESS_2_SHADER_BINDING_TABLE_READ_BIT_KHR;
    AccessMask AccessMasks::ComputeShader::samplerHeapRead = VK_ACCESS_2_SAMPLER_HEAP_READ_BIT_EXT;
    AccessMask AccessMasks::ComputeShader::resourceHeapRead = VK_ACCESS_2_RESOURCE_HEAP_READ_BIT_EXT;
    AccessMask AccessMasks::ComputeShader::shaderTileAttachmentRead = VK_ACCESS_2_SHADER_TILE_ATTACHMENT_READ_BIT_QCOM;
    AccessMask AccessMasks::ComputeShader::shaderTileAttachmentWrite = VK_ACCESS_2_SHADER_TILE_ATTACHMENT_WRITE_BIT_QCOM;
    AccessMask AccessMasks::ComputeShader::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT:
    AccessMask AccessMasks::VertexAttributeInput::vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
    AccessMask AccessMasks::VertexAttributeInput::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::VertexAttributeInput::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::VertexAttributeInput::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT:
    AccessMask AccessMasks::VertexInput::indexRead = VK_ACCESS_2_INDEX_READ_BIT;
    AccessMask AccessMasks::VertexInput::vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
    AccessMask AccessMasks::VertexInput::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::VertexInput::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::VertexInput::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT:
    AccessMask AccessMasks::VertexShader::uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
    AccessMask AccessMasks::VertexShader::shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
    AccessMask AccessMasks::VertexShader::shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
    AccessMask AccessMasks::VertexShader::sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
    AccessMask AccessMasks::VertexShader::storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    AccessMask AccessMasks::VertexShader::storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    AccessMask AccessMasks::VertexShader::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::VertexShader::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::VertexShader::accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    AccessMask AccessMasks::VertexShader::descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
    AccessMask AccessMasks::VertexShader::shaderBindingTableRead = VK_ACCESS_2_SHADER_BINDING_TABLE_READ_BIT_KHR;
    AccessMask AccessMasks::VertexShader::samplerHeapRead = VK_ACCESS_2_SAMPLER_HEAP_READ_BIT_EXT;
    AccessMask AccessMasks::VertexShader::resourceHeapRead = VK_ACCESS_2_RESOURCE_HEAP_READ_BIT_EXT;
    AccessMask AccessMasks::VertexShader::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT:
    AccessMask AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::EarlyFragmentTest::depthStencilAttachmentWrite = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    AccessMask AccessMasks::EarlyFragmentTest::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::EarlyFragmentTest::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::EarlyFragmentTest::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT:
    AccessMask AccessMasks::LateFragmentTest::depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::LateFragmentTest::depthStencilAttachmentWrite = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    AccessMask AccessMasks::LateFragmentTest::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::LateFragmentTest::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::LateFragmentTest::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT:
    AccessMask AccessMasks::FragmentShader::uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
    AccessMask AccessMasks::FragmentShader::inputAttachmentRead = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::FragmentShader::shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
    AccessMask AccessMasks::FragmentShader::shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
    AccessMask AccessMasks::FragmentShader::colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::FragmentShader::depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::FragmentShader::sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
    AccessMask AccessMasks::FragmentShader::storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    AccessMask AccessMasks::FragmentShader::storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    AccessMask AccessMasks::FragmentShader::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::FragmentShader::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::FragmentShader::accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    AccessMask AccessMasks::FragmentShader::descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
    AccessMask AccessMasks::FragmentShader::shaderBindingTableRead = VK_ACCESS_2_SHADER_BINDING_TABLE_READ_BIT_KHR;
    AccessMask AccessMasks::FragmentShader::samplerHeapRead = VK_ACCESS_2_SAMPLER_HEAP_READ_BIT_EXT;
    AccessMask AccessMasks::FragmentShader::resourceHeapRead = VK_ACCESS_2_RESOURCE_HEAP_READ_BIT_EXT;
    AccessMask AccessMasks::FragmentShader::shaderTileAttachmentRead = VK_ACCESS_2_SHADER_TILE_ATTACHMENT_READ_BIT_QCOM;
    AccessMask AccessMasks::FragmentShader::shaderTileAttachmentWrite = VK_ACCESS_2_SHADER_TILE_ATTACHMENT_WRITE_BIT_QCOM;
    AccessMask AccessMasks::FragmentShader::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT:
    AccessMask AccessMasks::ColorAttachmentOutput::colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::ColorAttachmentOutput::colorAttachmentWrite = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    AccessMask AccessMasks::ColorAttachmentOutput::colorAttachmentReadNoncoherent = VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT;
    AccessMask AccessMasks::ColorAttachmentOutput::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::ColorAttachmentOutput::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::ColorAttachmentOutput::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_COPY_BIT:
    AccessMask AccessMasks::Copy::transferRead = VK_ACCESS_2_TRANSFER_READ_BIT;
    AccessMask AccessMasks::Copy::transferWrite = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    AccessMask AccessMasks::Copy::memoryRead = VK_ACCESS_2_MEMORY_READ_BIT;
    AccessMask AccessMasks::Copy::memoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT;
    AccessMask AccessMasks::Copy::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_NONE:
    AccessMask AccessMasks::None::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT:
    AccessMask AccessMasks::BottomOfPipe::none = VK_ACCESS_2_NONE;

    std::string AccessMasks::ToString(AccessMask accessMask)
    {
        return emberVulkanUtility::ToString_VkAccessFlags2(accessMask);
    }
}