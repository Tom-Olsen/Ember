#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using ShaderStage = uint32_t;
    struct ShaderStages
    {
        static ShaderStage vertex_bit;
        static ShaderStage tessellation_control_bit;
        static ShaderStage tessellation_evaluation_bit;
        static ShaderStage geometry_bit;
        static ShaderStage fragment_bit;
        static ShaderStage compute_bit;
        static ShaderStage all_graphics;
        static ShaderStage all;
        static ShaderStage raygen_bit_khr;
        static ShaderStage any_hit_bit_khr;
        static ShaderStage closest_hit_bit_khr;
        static ShaderStage miss_bit_khr;
        static ShaderStage intersection_bit_khr;
        static ShaderStage callable_bit_khr;
        static ShaderStage task_bit_ext;
        static ShaderStage mesh_bit_ext;
        static ShaderStage subpass_shading_bit_huawei;
        static ShaderStage cluster_culling_bit_huawei;
        static ShaderStage flag_bits_max_enum;
        static std::string ToString(ShaderStage shaderStage);
    };
}