#include "vulkanVertexLayout.h"
#include "vulkanMesh.h"
#include <cstddef>
#include <stdexcept>



namespace vulkanRendererBackend
{
    // Helpers:
    Mesh::VertexAttribute GetVertexAttribute(const emberSpirvReflect::VertexAttributeInfo& vertexAttributeInfo)
    {
        const std::string& semantic = vertexAttributeInfo.semantic;
        if (semantic == "position" || semantic == "POSITION")
            return Mesh::VertexAttribute::position;
        if (semantic == "normal" || semantic == "NORMAL")
            return Mesh::VertexAttribute::normal;
        if (semantic == "tangent" || semantic == "TANGENT")
            return Mesh::VertexAttribute::tangent;
        if (semantic == "vertexColor" || semantic == "color" || semantic == "COLOR")
            return Mesh::VertexAttribute::color;
        if (semantic == "uv" || semantic == "TEXCOORD" || semantic == "TEXCOORD0")
            return Mesh::VertexAttribute::uv;

        throw std::runtime_error("Unsupported vertex attribute semantic: " + semantic);
    }
    uint32_t GetVertexStride(const emberSpirvReflect::VertexAttributeInfo& vertexAttributeInfo)
    {
        switch (GetVertexAttribute(vertexAttributeInfo))
        {
        case Mesh::VertexAttribute::position: return sizeof(Float3);
        case Mesh::VertexAttribute::normal:   return sizeof(Float3);
        case Mesh::VertexAttribute::tangent:  return sizeof(Float3);
        case Mesh::VertexAttribute::color:    return sizeof(Float4);
        case Mesh::VertexAttribute::uv:       return sizeof(Float4);
        default: throw std::runtime_error("Unsupported vertex attribute.");
        }
    }
    uint32_t GetInterleavedVertexOffset(const emberSpirvReflect::VertexAttributeInfo& vertexAttributeInfo)
    {
        switch (GetVertexAttribute(vertexAttributeInfo))
        {
        case Mesh::VertexAttribute::position: return static_cast<uint32_t>(offsetof(Mesh::Vertex, position));
        case Mesh::VertexAttribute::normal:   return static_cast<uint32_t>(offsetof(Mesh::Vertex, normal));
        case Mesh::VertexAttribute::tangent:  return static_cast<uint32_t>(offsetof(Mesh::Vertex, tangent));
        case Mesh::VertexAttribute::color:    return static_cast<uint32_t>(offsetof(Mesh::Vertex, color));
        case Mesh::VertexAttribute::uv:       return static_cast<uint32_t>(offsetof(Mesh::Vertex, uv));
        default: throw std::runtime_error("Unsupported vertex attribute.");
        }
    }



    // Methods:
	template<typename vertexLayout>
	std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions(const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos)
	{
        std::vector<VkVertexInputBindingDescription> bindings;
        if constexpr (std::is_same_v<vertexLayout, InterleavedVertexLayout>)
        {
            if (!vertexAttributeInfos.empty())
            {
                VkVertexInputBindingDescription binding;
                binding.binding = 0;
                binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                binding.stride = sizeof(Mesh::Vertex);
                bindings.push_back(binding);
            }
        }
        else if constexpr (std::is_same_v<vertexLayout, SeparateVertexLayout>)
        {
            for (const emberSpirvReflect::VertexAttributeInfo& vertexAttributeInfo : vertexAttributeInfos)
            {
                VkVertexInputBindingDescription binding;
                binding.binding = static_cast<uint32_t>(GetVertexAttribute(vertexAttributeInfo));
                binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                binding.stride = GetVertexStride(vertexAttributeInfo);
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
            for (const emberSpirvReflect::VertexAttributeInfo& vertexAttributeInfo : vertexAttributeInfos)
            {
                VkVertexInputAttributeDescription attribute;
                attribute.binding = 0;
                attribute.format = VkFormat(vertexAttributeInfo.format);
                attribute.location = vertexAttributeInfo.location;
                attribute.offset = GetInterleavedVertexOffset(vertexAttributeInfo);
                attributes.push_back(attribute);
            }
        }
        else if constexpr (std::is_same_v<vertexLayout, SeparateVertexLayout>)
        {
            for (const emberSpirvReflect::VertexAttributeInfo& vertexAttributeInfo : vertexAttributeInfos)
            {
                VkVertexInputAttributeDescription attribute;
                attribute.binding = static_cast<uint32_t>(GetVertexAttribute(vertexAttributeInfo));
                attribute.format = VkFormat(vertexAttributeInfo.format);
                attribute.location = vertexAttributeInfo.location;
                attribute.offset = 0;
                attributes.push_back(attribute);
            }
        }
        return attributes;
    }



    // Explicit template instantiation:
    template std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions<InterleavedVertexLayout>(const std::vector<emberSpirvReflect::VertexAttributeInfo>&);
    template std::vector<VkVertexInputBindingDescription> GetVertexBindingDescriptions<SeparateVertexLayout>(const std::vector<emberSpirvReflect::VertexAttributeInfo>&);
    template std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions<InterleavedVertexLayout>(const std::vector<emberSpirvReflect::VertexAttributeInfo>&);
    template std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions<SeparateVertexLayout>(const std::vector<emberSpirvReflect::VertexAttributeInfo>&);
}