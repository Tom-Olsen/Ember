#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



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
	std::unordered_map<std::string, UniformBufferBlock*> m_uniformBufferBlockMap;
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
	const std::vector<std::string>& GetBindingNames() const;
	const std::unordered_map<std::string, UniformBufferBlock*>& GetUniformBufferBlockMap() const;
	VulkanContext* const GetContext() const;
	uint32_t GetBindingCount() const;
	uint32_t GetBindingIndex(uint32_t i) const;
	VkDescriptorType GetBindingType(uint32_t i) const;
	const std::string& GetBindingName(uint32_t i) const;
	UniformBufferBlock* GetUniformBufferBlock(const std::string& name) const;

	// Debugging:
	void PrintBindings() const;
	void PrintUniformBuffers() const;

private: // Methods:
	static std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
};



#endif // __INCLUDE_GUARD_material_h__