#ifndef __INCLUDE_GUARD_shaderProperties_h__
#define __INCLUDE_GUARD_shaderProperties_h__
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class Shader;
	class Sampler;
	class StorageBuffer;
	class Texture2d;
	class UniformBuffer;



	// Resource bindings:
	struct UniformBufferBinding
	{
		uint32_t binding;
		std::shared_ptr<UniformBuffer> pUniformBuffer;
	};
	struct SamplerBinding
	{
		uint32_t binding;
		Sampler* pSampler;
	};
	struct TextureBinding
	{
		uint32_t binding;
		Texture2d* pTexture2d;
		VkDescriptorType descriptorType;
	};
	struct StorageBufferBinding
	{
		uint32_t binding;
		StorageBuffer* pStorageBuffer;
	};

	

	/// <summary>
	/// Each ShaderProperties instance is customized for a specific Material/ComputeShader.
	/// ShaderProperties construction is expensive, do not create them in an update loop.
	/// ShaderProperties own UniformBuffer pointers, Sampler and Texture2d pointers are owned by associated Managers.
	/// </summary>
	class ShaderProperties
	{
	private: // Members:
		Shader* m_pShader;

		// All these vectors contain one item for each frame in flight:
		std::vector<VkDescriptorSet> m_descriptorSets;
		std::vector<std::unordered_map<std::string, UniformBufferBinding>> m_uniformBufferMaps;
		std::vector<std::unordered_map<std::string, SamplerBinding>> m_samplerMaps;
		std::vector<std::unordered_map<std::string, TextureBinding>> m_texture2dMaps;
		std::vector<std::unordered_map<std::string, StorageBufferBinding>> m_storageBufferMaps;

		// UniformBuffer does not need stagingMap, as it contains a host and device buffer, where the host buffer is the staging buffer:
		std::unordered_map<std::string, Sampler*> m_samplerStagingMap;
		std::unordered_map<std::string, Texture2d*> m_texture2dStagingMap;
		std::unordered_map<std::string, StorageBuffer*> m_storageBufferStagingMap;

		// UniformBuffer instead needs one bool per frame in flight and per uniform buffer, to check if any values are updated:
		std::vector<std::unordered_map<std::string, bool>> m_updateUniformBuffer;

	public: // Methods:
		// Constructors/Destructor:
		ShaderProperties(Shader* pShader);
		~ShaderProperties();

		void UpdateShaderData();
		const VkDescriptorSet& GetDescriptorSet(uint32_t frameIndex);
		std::vector<VkDescriptorSet>& GetDescriptorSets();

		// Setters:
		void SetSampler(const std::string& name, Sampler* pSampler);
		void SetTexture2d(const std::string& name, Texture2d* pTexture2d);
		void SetStorageBuffer(const std::string& name, StorageBuffer* pStorageBuffer);
		// Uniform Buffer Setters:
		template<typename T>
		void SetValue(const std::string& bufferName, const std::string& memberName, const T& value);
		template<typename T>
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const T& value);
		template<typename T>
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value);
		template<typename T>
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value);

		// Getters:
		Shader* GetShader() const;
		Sampler* GetSampler(const std::string& name) const;
		Texture2d* GetTexture2d(const std::string& name) const;

		// Uniform Buffer Getters:
		template<typename T>
		T GetValue(const std::string& bufferName, const std::string& memberName) const;
		template<typename T>
		T GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		template<typename T>
		T GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		template<typename T>
		T GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;

		// Debugging:
		void Print(const std::string& name) const;
		void PrintMaps() const;

	private: // Methods:
		// Initializers:
		void InitUniformBufferResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding);
		void InitSamplerResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Sampler* pSampler);
		void InitTexture2dResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Texture2d* pTexture2d, VkDescriptorType descriptorType);
		void InitStorageBufferResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, StorageBuffer* pStorageBuffer);
		void InitStagingMaps();
		void InitDescriptorSets();

		// Descriptor Set management:
		void CreateDescriptorSets();
		void UpdateDescriptorSet(uint32_t frameIndex, UniformBufferBinding samplerResourceBinding);
		void UpdateDescriptorSet(uint32_t frameIndex, SamplerBinding samplerResourceBinding);
		void UpdateDescriptorSet(uint32_t frameIndex, TextureBinding texture2dResourceBinding);
		void UpdateDescriptorSet(uint32_t frameIndex, StorageBufferBinding storageBufferResourceBinding);
	};
}



#endif // __INCLUDE_GUARD_shaderProperties_h__