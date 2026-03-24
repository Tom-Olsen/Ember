#include "vulkanVertexLayout.h"



namespace vulkanRendererBackend
{
    // Ember::ToDo: this needs verification!

	template<typename vertexLayout>
	std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions(const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos)
	{
        std::vector<VkVertexInputBindingDescription> bindings;
        if constexpr (std::is_same_v<vertexLayout, InterleavedVertexLayout>)
        {
            uint32_t stride = 0;
            for (const emberSpirvReflect::VertexAttributeInfo& vertexAttributeInfo : vertexAttributeInfos)
                stride += vertexAttributeInfo.typeSize * vertexAttributeInfo.vectorSize;

            VkVertexInputBindingDescription binding;
            binding.binding = 0;
            binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            binding.stride = stride;
            bindings.push_back(binding);
        }
        else if constexpr (std::is_same_v<vertexLayout, SeparateVertexLayout>)
        {
            for (int i = 0; i < vertexAttributeInfos.size(); i++)
            {
                VkVertexInputBindingDescription binding;
                binding.binding = i;
                binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                binding.stride = vertexAttributeInfos[i].typeSize * vertexAttributeInfos[i].vectorSize;
                bindings.push_back(binding);
            }
        }
        return bindings;
	}
    template<typename vertexLayout>
    std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions(const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos)
    {
        std::vector<VkVertexInputAttributeDescription> attributes;
        if constexpr (std::is_same_v<vertexLayout, InterleavedVertexLayout>)
        {
            uint32_t offset = 0;
            for (const emberSpirvReflect::VertexAttributeInfo& vertexAttributeInfo : vertexAttributeInfos)
            {
                VkVertexInputAttributeDescription attribute;
                attribute.binding = 0;
                attribute.format = vertexAttributeInfo.format;
                attribute.location = vertexAttributeInfo.location;
                attribute.offset = offset;
                attributes.push_back(attribute);
                offset += vertexAttributeInfo.typeSize * vertexAttributeInfo.vectorSize;
            }
        }
        else if constexpr (std::is_same_v<vertexLayout, SeparateVertexLayout>)
        {
            for (int i = 0; i < vertexAttributeInfos.size(); i++)
            {
                VkVertexInputAttributeDescription attribute;
                attribute.binding = i;
                attribute.format = vertexAttributeInfos[i].format;
                attribute.location = vertexAttributeInfos[i].location;
                attribute.offset = 0;
                attributes.push_back(attribute);
            }
        }
        return attributes;
    }



    // Explicit template instantiation:
    template std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions<InterleavedVertexLayout>(const std::vector<emberSpirvReflect::VertexAttributeInfo>&);
    template std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions<SeparateVertexLayout>(const std::vector<emberSpirvReflect::VertexAttributeInfo>&);
    template std::vector<VkVertexInputBindingDescription> GetVertexAttributeDescriptions<InterleavedVertexLayout>(const std::vector<emberSpirvReflect::VertexAttributeInfo>&);
    template std::vector<VkVertexInputBindingDescription> GetVertexAttributeDescriptions<SeparateVertexLayout>(const std::vector<emberSpirvReflect::VertexAttributeInfo>&);
}