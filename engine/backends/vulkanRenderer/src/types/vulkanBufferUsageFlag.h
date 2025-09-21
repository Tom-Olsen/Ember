#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using BufferUsageFlag = uint32_t;
    struct BufferUsageFlags
    {
        static BufferUsageFlag transfer_src_bit;
        static BufferUsageFlag transfer_dst_bit;
        static BufferUsageFlag uniform_texel_buffer_bit;
        static BufferUsageFlag storage_texel_buffer_bit;
        static BufferUsageFlag uniform_buffer_bit;
        static BufferUsageFlag storage_buffer_bit;
        static BufferUsageFlag index_buffer_bit;
        static BufferUsageFlag vertex_buffer_bit;
        static BufferUsageFlag indirect_buffer_bit;
        static BufferUsageFlag shader_device_address_bit;
        static BufferUsageFlag video_decode_src_bit_khr;
        static BufferUsageFlag video_decode_dst_bit_khr;
        static BufferUsageFlag transform_feedback_buffer_bit_ext;
        static BufferUsageFlag transform_feedback_counter_buffer_bit_ext;
        static BufferUsageFlag conditional_rendering_bit_ext;
        #ifdef VK_ENABLE_BETA_EXTENSIONS
        static BufferUsageFlag execution_graph_scratch_bit_amdx;
        #endif
        static BufferUsageFlag acceleration_structure_build_input_read_only_bit_khr;
        static BufferUsageFlag acceleration_structure_storage_bit_khr;
        static BufferUsageFlag shader_binding_table_bit_khr;
        static BufferUsageFlag video_encode_dst_bit_khr;
        static BufferUsageFlag video_encode_src_bit_khr;
        static BufferUsageFlag sampler_descriptor_buffer_bit_ext;
        static BufferUsageFlag resource_descriptor_buffer_bit_ext;
        static BufferUsageFlag push_descriptors_descriptor_buffer_bit_ext;
        static BufferUsageFlag micromap_build_input_read_only_bit_ext;
        static BufferUsageFlag micromap_storage_bit_ext;
        static BufferUsageFlag flag_bits_max_enum;
        static std::string ToString(BufferUsageFlag bufferUsageFlag);
    };
}