#include "vulkanVertexLayout.h"



namespace vulkanRendererBackend
{
	template<typename vertexLayout>
	std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions(const std::vector<emberSpirvReflect::VertexStageInfo>& vertexStageInfos)
	{
        std::vector<VkVertexInputBindingDescription> bindings;
        if constexpr (std::is_same_v<vertexLayout, InterleavedVertexLayout>)
        {
            static_assert(false, "vulkanRendererBackend::GetVertexBindingDescriptions(): interleaved layout not implemented yet.");
        }
        else if constexpr (std::is_same_v<vertexLayout, SeparateVertexLayout>)
        {
            for (int i = 0; i < vertexStageInfos.size(); i++)
            {
                VkVertexInputBindingDescription binding;
                binding.binding = i;
                binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                binding.stride = vertexStageInfos[i].typeSize * vertexStageInfos[i].vectorSize;
                bindings.push_back(binding);
            }
        }
        else
        {
            static_assert(false, "vulkanRendererBackend::GetVertexBindingDescriptions(): Unsupported vertex layout.");
        }
        return bindings;
	}
    template<typename vertexLayout>
    std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions(const std::vector<emberSpirvReflect::VertexStageInfo>& vertexStageInfos)
    {
        std::vector<VkVertexInputAttributeDescription> attributes;
        if constexpr (std::is_same_v<vertexLayout, InterleavedVertexLayout>)
        {
            static_assert(false, "vulkanRendererBackend::GetVertexAttributeDescriptions(): interleaved layout not implemented yet.");
        }
        else if constexpr (std::is_same_v<vertexLayout, SeparateVertexLayout>)
        {
            for (int i = 0; i < vertexStageInfos.size(); i++)
            {
                VkVertexInputAttributeDescription attribute;
                attribute.binding = i;
                attribute.format = vertexStageInfos[i].format;
                attribute.location = vertexStageInfos[i].location;
                attribute.offset = 0;
                attributes.push_back(attribute);
            }
        }
        else
        {
            static_assert(false, "vulkanRendererBackend::GetVertexAttributeDescriptions(): Unsupported vertex layout.");
        }
        return attributes;
    }



    // Explicit template instantiation:
    template std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions<InterleavedVertexLayout>( const std::vector<emberSpirvReflect::VertexStageInfo>&);
    template std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions<SeparateVertexLayout>( const std::vector<emberSpirvReflect::VertexStageInfo>&);
    template std::vector<VkVertexInputBindingDescription> GetVertexAttributeDescriptions<InterleavedVertexLayout>(const std::vector<emberSpirvReflect::VertexStageInfo>&);
    template std::vector<VkVertexInputBindingDescription> GetVertexAttributeDescriptions<SeparateVertexLayout>(const std::vector<emberSpirvReflect::VertexStageInfo>&);
}