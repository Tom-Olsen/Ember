#include "vulkanBufferUsageFlag.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    BufferUsageFlag BufferUsageFlags::transfer_src_bit = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    BufferUsageFlag BufferUsageFlags::transfer_dst_bit = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    BufferUsageFlag BufferUsageFlags::uniform_texel_buffer_bit = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
    BufferUsageFlag BufferUsageFlags::storage_texel_buffer_bit = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
    BufferUsageFlag BufferUsageFlags::uniform_buffer_bit = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    BufferUsageFlag BufferUsageFlags::storage_buffer_bit = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    BufferUsageFlag BufferUsageFlags::index_buffer_bit = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    BufferUsageFlag BufferUsageFlags::vertex_buffer_bit = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    BufferUsageFlag BufferUsageFlags::indirect_buffer_bit = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    BufferUsageFlag BufferUsageFlags::shader_device_address_bit = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    BufferUsageFlag BufferUsageFlags::video_decode_src_bit_khr = VK_BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR;
    BufferUsageFlag BufferUsageFlags::video_decode_dst_bit_khr = VK_BUFFER_USAGE_VIDEO_DECODE_DST_BIT_KHR;
    BufferUsageFlag BufferUsageFlags::transform_feedback_buffer_bit_ext = VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT;
    BufferUsageFlag BufferUsageFlags::transform_feedback_counter_buffer_bit_ext = VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT;
    BufferUsageFlag BufferUsageFlags::conditional_rendering_bit_ext = VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT;
    #ifdef VK_ENABLE_BETA_EXTENSIONS
    BufferUsageFlag BufferUsageFlags::execution_graph_scratch_bit_amdx = VK_BUFFER_USAGE_EXECUTION_GRAPH_SCRATCH_BIT_AMDX;
    #endif
    BufferUsageFlag BufferUsageFlags::acceleration_structure_build_input_read_only_bit_khr = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
    BufferUsageFlag BufferUsageFlags::acceleration_structure_storage_bit_khr = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
    BufferUsageFlag BufferUsageFlags::shader_binding_table_bit_khr = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;
    BufferUsageFlag BufferUsageFlags::video_encode_dst_bit_khr = VK_BUFFER_USAGE_VIDEO_ENCODE_DST_BIT_KHR;
    BufferUsageFlag BufferUsageFlags::video_encode_src_bit_khr = VK_BUFFER_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;
    BufferUsageFlag BufferUsageFlags::sampler_descriptor_buffer_bit_ext = VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;
    BufferUsageFlag BufferUsageFlags::resource_descriptor_buffer_bit_ext = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
    BufferUsageFlag BufferUsageFlags::push_descriptors_descriptor_buffer_bit_ext = VK_BUFFER_USAGE_PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER_BIT_EXT;
    BufferUsageFlag BufferUsageFlags::micromap_build_input_read_only_bit_ext = VK_BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY_BIT_EXT;
    BufferUsageFlag BufferUsageFlags::micromap_storage_bit_ext = VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT;
    BufferUsageFlag BufferUsageFlags::flag_bits_max_enum = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;

    std::string BufferUsageFlags::ToString(BufferUsageFlag bufferUsageFlag)
    {
        if (bufferUsageFlag == 0)
            return "none";

        std::string result;

        auto append = [&](BufferUsageFlag bit, std::string name)
        {
            if (bufferUsageFlag & bit)
            {
                if (!result.empty()) result += " | ";
                result += name;
            }
        };

        append(BufferUsageFlags::transfer_src_bit, "transfer_src_bit");
        append(BufferUsageFlags::transfer_dst_bit, "transfer_dst_bit");
        append(BufferUsageFlags::uniform_texel_buffer_bit, "uniform_texel_buffer_bit");
        append(BufferUsageFlags::storage_texel_buffer_bit, "storage_texel_buffer_bit");
        append(BufferUsageFlags::uniform_buffer_bit, "uniform_buffer_bit");
        append(BufferUsageFlags::storage_buffer_bit, "storage_buffer_bit");
        append(BufferUsageFlags::index_buffer_bit, "index_buffer_bit");
        append(BufferUsageFlags::vertex_buffer_bit, "vertex_buffer_bit");
        append(BufferUsageFlags::indirect_buffer_bit, "indirect_buffer_bit");
        append(BufferUsageFlags::shader_device_address_bit, "shader_device_address_bit");
        append(BufferUsageFlags::video_decode_src_bit_khr, "video_decode_src_bit_khr");
        append(BufferUsageFlags::video_decode_dst_bit_khr, "video_decode_dst_bit_khr");
        append(BufferUsageFlags::transform_feedback_buffer_bit_ext, "transform_feedback_buffer_bit_ext");
        append(BufferUsageFlags::transform_feedback_counter_buffer_bit_ext, "transform_feedback_counter_buffer_bit_ext");
        append(BufferUsageFlags::conditional_rendering_bit_ext, "conditional_rendering_bit_ext");
        #ifdef VK_ENABLE_BETA_EXTENSIONS
        append(BufferUsageFlags::execution_graph_scratch_bit_amdx, "execution_graph_scratch_bit_amdx");
        #endif
        append(BufferUsageFlags::acceleration_structure_build_input_read_only_bit_khr, "acceleration_structure_build_input_read_only_bit_khr");
        append(BufferUsageFlags::acceleration_structure_storage_bit_khr, "acceleration_structure_storage_bit_khr");
        append(BufferUsageFlags::shader_binding_table_bit_khr, "shader_binding_table_bit_khr");
        append(BufferUsageFlags::video_encode_dst_bit_khr, "video_encode_dst_bit_khr");
        append(BufferUsageFlags::video_encode_src_bit_khr, "video_encode_src_bit_khr");
        append(BufferUsageFlags::sampler_descriptor_buffer_bit_ext, "sampler_descriptor_buffer_bit_ext");
        append(BufferUsageFlags::resource_descriptor_buffer_bit_ext, "resource_descriptor_buffer_bit_ext");
        append(BufferUsageFlags::push_descriptors_descriptor_buffer_bit_ext, "push_descriptors_descriptor_buffer_bit_ext");
        append(BufferUsageFlags::micromap_build_input_read_only_bit_ext, "micromap_build_input_read_only_bit_ext");
        append(BufferUsageFlags::micromap_storage_bit_ext, "micromap_storage_bit_ext");
        append(BufferUsageFlags::flag_bits_max_enum, "flag_bits_max_enum");

        return result.empty() ? "unknown" : result;
    }
}