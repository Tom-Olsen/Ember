#include "vulkanShaderStage.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	ShaderStage ShaderStages::vertex_bit = VK_SHADER_STAGE_VERTEX_BIT;
	ShaderStage ShaderStages::tessellation_control_bit = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	ShaderStage ShaderStages::tessellation_evaluation_bit = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	ShaderStage ShaderStages::geometry_bit = VK_SHADER_STAGE_GEOMETRY_BIT;
	ShaderStage ShaderStages::fragment_bit = VK_SHADER_STAGE_FRAGMENT_BIT;
	ShaderStage ShaderStages::compute_bit = VK_SHADER_STAGE_COMPUTE_BIT;
	ShaderStage ShaderStages::all_graphics = VK_SHADER_STAGE_ALL_GRAPHICS;
	ShaderStage ShaderStages::all = VK_SHADER_STAGE_ALL;
	ShaderStage ShaderStages::raygen_bit_khr = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	ShaderStage ShaderStages::any_hit_bit_khr = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
	ShaderStage ShaderStages::closest_hit_bit_khr = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	ShaderStage ShaderStages::miss_bit_khr = VK_SHADER_STAGE_MISS_BIT_KHR;
	ShaderStage ShaderStages::intersection_bit_khr = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
	ShaderStage ShaderStages::callable_bit_khr = VK_SHADER_STAGE_CALLABLE_BIT_KHR;
	ShaderStage ShaderStages::task_bit_ext = VK_SHADER_STAGE_TASK_BIT_EXT;
	ShaderStage ShaderStages::mesh_bit_ext = VK_SHADER_STAGE_MESH_BIT_EXT;
	ShaderStage ShaderStages::subpass_shading_bit_huawei = VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI;
	ShaderStage ShaderStages::cluster_culling_bit_huawei = VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI;
	ShaderStage ShaderStages::flag_bits_max_enum = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

	std::string ShaderStages::ToString(ShaderStage shaderStage)
	{
		if (shaderStage == 0)
			return "none";

		std::string result;

		auto append = [&](ShaderStage bit, std::string name)
		{
			if (shaderStage & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

		append(ShaderStages::vertex_bit, "vertex_bit");
		append(ShaderStages::tessellation_control_bit, "tessellation_control_bit");
		append(ShaderStages::tessellation_evaluation_bit, "tessellation_evaluation_bit");
		append(ShaderStages::geometry_bit, "geometry_bit");
		append(ShaderStages::fragment_bit, "fragment_bit");
		append(ShaderStages::compute_bit, "compute_bit");
		append(ShaderStages::raygen_bit_khr, "raygen_bit_khr");
		append(ShaderStages::any_hit_bit_khr, "any_hit_bit_khr");
		append(ShaderStages::closest_hit_bit_khr, "closest_hit_bit_khr");
		append(ShaderStages::miss_bit_khr, "miss_bit_khr");
		append(ShaderStages::intersection_bit_khr, "intersection_bit_khr");
		append(ShaderStages::callable_bit_khr, "callable_bit_khr");
		append(ShaderStages::task_bit_ext, "task_bit_ext");
		append(ShaderStages::mesh_bit_ext, "mesh_bit_ext");
		append(ShaderStages::subpass_shading_bit_huawei, "subpass_shading_bit_huawei");
		append(ShaderStages::cluster_culling_bit_huawei, "cluster_culling_bit_huawei");
		append(ShaderStages::flag_bits_max_enum, "flag_bits_max_enum");

		return result.empty() ? "unknown" : result;
	}
}