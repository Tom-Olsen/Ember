#pragma once
#include "shaderReflection.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Template parameter structs for vertex layout:
	struct InterleavedVertexLayout {};
	struct SeparateVertexLayout {};

	template<typename vertexLayout>
	std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions(const std::vector<emberSpirvReflect::VertexStageInfo>& vertexStageInfos);
	template<typename vertexLayout>
	std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions(const std::vector<emberSpirvReflect::VertexStageInfo>& vertexStageInfos);
}