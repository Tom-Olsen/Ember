#include "vulkanAccessMask.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT:
    AccessMask AccessMasks::TopOfPipe::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_TRANSFER_BIT:
    AccessMask AccessMasks::Transfer::transferRead = VK_ACCESS_2_TRANSFER_READ_BIT;
    AccessMask AccessMasks::Transfer::transferWrite = VK_ACCESS_2_TRANSFER_WRITE_BIT;
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
    AccessMask AccessMasks::ComputeShader::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT:
    AccessMask AccessMasks::VertexAttributeInput::vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
    AccessMask AccessMasks::VertexAttributeInput::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT:
    AccessMask AccessMasks::VertexInput::indexRead = VK_ACCESS_2_INDEX_READ_BIT;
    AccessMask AccessMasks::VertexInput::vertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
    AccessMask AccessMasks::VertexInput::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT:
    AccessMask AccessMasks::VertexShader::uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
    AccessMask AccessMasks::VertexShader::shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
    AccessMask AccessMasks::VertexShader::shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
    AccessMask AccessMasks::VertexShader::sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
    AccessMask AccessMasks::VertexShader::storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    AccessMask AccessMasks::VertexShader::storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    AccessMask AccessMasks::VertexShader::accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    AccessMask AccessMasks::VertexShader::descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
    AccessMask AccessMasks::VertexShader::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT:
    AccessMask AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::EarlyFragmentTest::depthStencilAttachmentWrite = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    AccessMask AccessMasks::EarlyFragmentTest::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT:
    AccessMask AccessMasks::FragmentShader::uniformRead = VK_ACCESS_2_UNIFORM_READ_BIT;
    AccessMask AccessMasks::FragmentShader::shaderRead = VK_ACCESS_2_SHADER_READ_BIT;
    AccessMask AccessMasks::FragmentShader::shaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT;
    AccessMask AccessMasks::FragmentShader::colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::FragmentShader::depthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::FragmentShader::sampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
    AccessMask AccessMasks::FragmentShader::storageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    AccessMask AccessMasks::FragmentShader::storageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    AccessMask AccessMasks::FragmentShader::accelerationStructureRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    AccessMask AccessMasks::FragmentShader::descriptorBufferRead = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
    AccessMask AccessMasks::FragmentShader::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT:
    AccessMask AccessMasks::ColorAttachmentOutput::colorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
    AccessMask AccessMasks::ColorAttachmentOutput::colorAttachmentWrite = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    AccessMask AccessMasks::ColorAttachmentOutput::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_COPY_BIT:
    AccessMask AccessMasks::Copy::transferRead = VK_ACCESS_2_TRANSFER_READ_BIT;
    AccessMask AccessMasks::Copy::transferWrite = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    AccessMask AccessMasks::Copy::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_NONE:
    AccessMask AccessMasks::None::none = VK_ACCESS_2_NONE;

    // VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT:
    AccessMask AccessMasks::BottomOfPipe::none = VK_ACCESS_2_NONE;

    std::string AccessMasks::ToString(AccessMask accessMask)
    {
        if (accessMask == 0)
            return "none";

        std::string result;

        auto append = [&](AccessMask bit, std::string name)
        {
            if (accessMask & bit)
            {
                if (!result.empty()) result += " | ";
                result += name;
            }
        };

        append(static_cast<AccessMask>(VK_ACCESS_2_NONE), "none");
        append(static_cast<AccessMask>(VK_ACCESS_2_TRANSFER_READ_BIT), "transfer_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_TRANSFER_WRITE_BIT), "transfer_write_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_UNIFORM_READ_BIT), "uniform_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_SHADER_READ_BIT), "shader_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_SHADER_WRITE_BIT), "shader_write_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_SHADER_SAMPLED_READ_BIT), "shader_sampled_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_SHADER_STORAGE_READ_BIT), "shader_storage_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT), "shader_storage_write_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_MEMORY_READ_BIT), "memory_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_MEMORY_WRITE_BIT), "memory_write_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR), "acceleration_structure_read_bit_khr");
        append(static_cast<AccessMask>(VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT), "descriptor_buffer_read_bit_ext");
        append(static_cast<AccessMask>(VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT), "vertex_attribute_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_INDEX_READ_BIT), "index_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT), "depth_stencil_attachment_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT), "depth_stencil_attachment_write_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT), "color_attachment_read_bit");
        append(static_cast<AccessMask>(VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT), "color_attachment_write_bit");

        return result.empty() ? "unknown" : result;
    }
}