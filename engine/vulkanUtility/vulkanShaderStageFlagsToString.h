#pragma once
#include <sstream>
#include <vulkan/vulkan.h>



namespace emberVulkanUtility
{
	inline std::string ToString(VkShaderStageFlags vkShaderStageFlags)
	{
        if (vkShaderStageFlags == 0)
            return "none";
        // These are not bits and need special handling:
        if (vkShaderStageFlags == VK_SHADER_STAGE_ALL_GRAPHICS)
            return "VK_SHADER_STAGE_ALL_GRAPHICS";
        if (vkShaderStageFlags == VK_SHADER_STAGE_ALL)
            return "VK_SHADER_STAGE_ALL";

        std::ostringstream ss;
        bool first = true;
        auto append = [&](VkShaderStageFlagBits bit, const char* name)
        {
            if (vkShaderStageFlags& bit)
            {
                if (!first)
                    ss << " | ";
                first = false;
                ss << name;
            }
        };

        append(VK_SHADER_STAGE_VERTEX_BIT, "VK_SHADER_STAGE_VERTEX_BIT");
        append(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT");
        append(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT");
        append(VK_SHADER_STAGE_GEOMETRY_BIT, "VK_SHADER_STAGE_GEOMETRY_BIT");
        append(VK_SHADER_STAGE_FRAGMENT_BIT, "VK_SHADER_STAGE_FRAGMENT_BIT");
        append(VK_SHADER_STAGE_COMPUTE_BIT, "VK_SHADER_STAGE_COMPUTE_BIT");
        append(VK_SHADER_STAGE_RAYGEN_BIT_KHR, "VK_SHADER_STAGE_RAYGEN_BIT_(KHR/NV)");
        append(VK_SHADER_STAGE_ANY_HIT_BIT_KHR, "VK_SHADER_STAGE_ANY_HIT_BIT_(KHR/NV)");
        append(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, "VK_SHADER_STAGE_CLOSEST_HIT_BIT_(KHR/NV)");
        append(VK_SHADER_STAGE_MISS_BIT_KHR, "VK_SHADER_STAGE_MISS_BIT_(KHR/NV)");
        append(VK_SHADER_STAGE_INTERSECTION_BIT_KHR, "VK_SHADER_STAGE_INTERSECTION_BIT_(KHR/NV)");
        append(VK_SHADER_STAGE_CALLABLE_BIT_KHR, "VK_SHADER_STAGE_CALLABLE_BIT_(KHR/NV)");
        append(VK_SHADER_STAGE_TASK_BIT_EXT, "VK_SHADER_STAGE_TASK_BIT_(EXT/NV)");
        append(VK_SHADER_STAGE_MESH_BIT_EXT, "VK_SHADER_STAGE_MESH_BIT_(EXT/NV)");
        append(VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI, "VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI");
        append(VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI, "VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI");

        if (first)
            ss << "UNKNOWN_STAGE_BITS(0x" << std::hex << static_cast<uint32_t>(vkShaderStageFlags) << std::dec << ")";

        return ss.str();
	}
}