#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



class Mesh;
class Pipeline;
struct VulkanContext;
struct UniformBufferBlock;



/// <summary>
/// Material creation is expensive.
/// It is recommended to create all materials at the start of the application.
/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
/// Each material must be used with a MaterialProperties which is customized for the Material.
/// </summary>
class Material
{
public: // Enums:
	enum class Type
	{
		shadow,
		shading,
		skybox
	};
	enum class RenderQueue
	{
		shadow = 0,
		opaque = 1000,
		transparent = 2000,
		skybox = 3000
	};

private: // Members:
	Type m_type;
	std::string m_name;
	RenderQueue m_renderQueue;
	std::unique_ptr<Pipeline> m_pPipeline;
	std::vector<VkDescriptorSetLayoutBinding> m_bindings;
	std::vector<std::string> m_bindingNames;
	std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
	std::vector<std::string> m_vertexInputNames;
	std::unordered_map<std::string, UniformBufferBlock*> m_uniformBufferBlockMap;
	std::vector<VkBuffer> m_meshBuffers;
	std::vector<VkDeviceSize> m_meshOffsets;
	VulkanContext* m_pContext;

public: // Methods:
	// Constructors/Destructor:
	Material(VulkanContext* pContext, Type type, const std::string& name, RenderQueue renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
	~Material();

	// Getters:
	Type GetType() const;
	const std::string& GetName() const;
	RenderQueue GetRenderQueue() const;
	const Pipeline* const GetPipeline() const;
	const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const;
	const std::vector<VkVertexInputBindingDescription>& GetBindingDescriptions() const;
	const std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() const;
	const std::vector<std::string>& GetBindingNames() const;
	const std::unordered_map<std::string, UniformBufferBlock*>& GetUniformBufferBlockMap() const;
	const uint32_t GetInputBindingCount() const;
	const VkBuffer* const GetMeshBuffers(Mesh* pMesh);
	const VkDeviceSize* const GetMeshOffsets(Mesh* pMesh);
	VulkanContext* const GetContext() const;
	uint32_t GetDescriptorBindingCount() const;
	uint32_t GetDescriptorBindingIndex(uint32_t i) const;
	VkDescriptorType GetDescriptorBindingType(uint32_t i) const;
	const std::string& GetDescriptorBindingName(uint32_t i) const;
	UniformBufferBlock* GetUniformBufferBlock(const std::string& name) const;

	// Debugging:
	void PrintBindings() const;
	void PrintUniformBuffers() const;

private: // Methods:
	static std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
};



#endif // __INCLUDE_GUARD_material_h__