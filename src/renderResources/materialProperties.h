#ifndef __INCLUDE_GUARD_materialProperties_h__
#define __INCLUDE_GUARD_materialProperties_h__
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



class Material;
class Sampler;
class Texture2d;
class UniformBuffer;
struct VulkanContext;



template<typename T>
struct ResourceBinding
{
	T resource;
	uint32_t binding;
};



/// <summary>
/// Each MaterialProperties instance is customized for a specific Material.
/// MaterialProperties construction is expensive, do not create them in an update loop.
/// MaterialProperties own UniformBuffer pointers, Sampler and Texture2d pointers are owned by associated Managers.
/// </summary>
class MaterialProperties
{
private: // Members:
	Material* m_pMaterial;
	VulkanContext* m_pContext;
	std::vector<VkDescriptorSet> m_descriptorSets;
	std::vector<std::unordered_map<std::string, ResourceBinding<std::shared_ptr<UniformBuffer>>>> m_uniformBufferMaps;
	std::vector<std::unordered_map<std::string, ResourceBinding<Sampler*>>> m_samplerMaps;
	std::vector<std::unordered_map<std::string, ResourceBinding<Texture2d*>>> m_texture2dMaps;
	std::unordered_map<std::string, Sampler*> m_samplerStagingMap;
	std::unordered_map<std::string, Texture2d*> m_texture2dStagingMap;
	std::vector<std::unordered_map<std::string, bool>> m_updateUniformBuffer;

public: // Methods:
	// Constructors/Destructor:
	MaterialProperties(Material* pMaterial);
	~MaterialProperties();

	void UpdateShaderData();
	const std::vector<VkDescriptorSet>& GetDescriptorSets() const;

	// Uniform Buffer Setters:
	template<typename T>
	void SetValue(const std::string& blockName, const std::string& memberName, const T& value);
	template<typename T>
	void SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const T& value);
	template<typename T>
	void SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value);
	template<typename T>
	void SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value);
	void SetSampler(const std::string& name, Sampler* pSampler);
	void SetTexture2d(const std::string& name, Texture2d* pTexture2d);

	// Uniform Buffer Getters:
	template<typename T>
	T GetValue(const std::string& blockName, const std::string& memberName) const;
	template<typename T>
	T GetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const;
	template<typename T>
	T GetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
	template<typename T>
	T GetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;

	// Debugging:
	void PrintMaps() const;

private: // Methods:
	// Initializers:
	void InitUniformBufferResourceBinding(const std::string& name, uint32_t binding, uint32_t frameIndex);
	void InitSamplerResourceBinding(const std::string& name, uint32_t binding, Sampler* pSampler, uint32_t frameIndex);
	void InitTexture2dResourceBinding(const std::string& name, uint32_t binding, Texture2d* pTexture2d, uint32_t frameIndex);
	void InitStagingMaps();
	void InitDescriptorSets();

	// Descriptor Set management:
	void CreateDescriptorSets();
	void UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<std::shared_ptr<UniformBuffer>> samplerResourceBinding);
	void UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<Sampler*> samplerResourceBinding);
	void UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<Texture2d*> texture2dResourceBinding);
};



#endif // __INCLUDE_GUARD_materialProperties_h__