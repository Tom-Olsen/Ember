#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using DescriptorType = uint32_t;
    struct DescriptorTypes
    {
        static DescriptorType sampler;
        static DescriptorType combined_image_sampler;
        static DescriptorType sampled_image;
        static DescriptorType storage_image;
        static DescriptorType uniform_texel_buffer;
        static DescriptorType storage_texel_buffer;
        static DescriptorType uniform_buffer;
        static DescriptorType storage_buffer;
        static DescriptorType uniform_buffer_dynamic;
        static DescriptorType storage_buffer_dynamic;
        static DescriptorType input_attachment;
        static DescriptorType inline_uniform_block;
        static DescriptorType acceleration_structure_khr;
        static DescriptorType acceleration_structure_nv;
        static DescriptorType sample_weight_image_qcom;
        static DescriptorType block_match_image_qcom;
        static DescriptorType mutable_ext;
        static DescriptorType max_enum;
        static std::string ToString(DescriptorType descriptorType);
    };
}