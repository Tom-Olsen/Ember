#pragma once
#include "iShaderProperties.h"
#include "vulkanDescriptorType.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



// Forward declarations:
typedef struct VkDescriptorSet_T* VkDescriptorSet;
namespace emberBackendInterface
{
	class ITexture;
	class IBuffer;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Shader;
	class Sampler;
	class Buffer;
	class Texture;
	class UniformBuffer;



	// Resource bindings:
	struct UniformBufferBinding	// UniformBuffer is a Buffer, but gets handled differently than other buffers as it owns cpu side memory which must be uploaded to gpu on update.
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
		Texture* pTexture;
		DescriptorType descriptorType;
	};
	struct BufferBinding
	{
		uint32_t binding;
		Buffer* pBuffer;
		DescriptorType descriptorType;
	};



	/// <summary>
	/// Each ShaderProperties instance is customized for a specific Material/ComputeShader.
	/// ShaderProperties construction is expensive, do not create them in an update loop.
	/// ShaderProperties own UniformBuffer pointers, Sampler and Texture pointers are not owned by ShaderProperties.
	/// </summary>
	class VULKAN_RENDERER_API ShaderProperties : public emberBackendInterface::IShaderProperties
	{
	private: // Members:
		Shader* m_pShader;

		// All these vectors contain one item for each frame in flight:
		std::vector<VkDescriptorSet> m_descriptorSets;
		std::vector<std::unordered_map<std::string, UniformBufferBinding>> m_uniformBufferMaps;
		std::vector<std::unordered_map<std::string, SamplerBinding>> m_samplerMaps;
		std::vector<std::unordered_map<std::string, TextureBinding>> m_textureMaps;
		std::vector<std::unordered_map<std::string, BufferBinding>> m_bufferMaps;

		// UniformBuffer does not need stagingMap, as it contains a host and device buffer, where the host buffer acts as a staging buffer:
		std::unordered_map<std::string, Sampler*> m_samplerStagingMap;
		std::unordered_map<std::string, Texture*> m_textureStagingMap;
		std::unordered_map<std::string, Buffer*> m_bufferStagingMap;

		// UniformBuffer instead needs one bool per frame in flight and per uniform buffer, to check if any values are updated:
		std::vector<std::unordered_map<std::string, bool>> m_updateUniformBuffer;

	public: // Methods:
		// Constructors/Destructor:
		ShaderProperties(Shader* pShader);
		~ShaderProperties();

		// Non-copyable:
		ShaderProperties(const ShaderProperties& other) = delete;
		ShaderProperties& operator=(const ShaderProperties& other) = delete;

		// Movable:
		ShaderProperties(ShaderProperties&& other) noexcept;
		ShaderProperties& operator=(ShaderProperties&& other) noexcept;

		// Setters:
		void SetSampler(const std::string& name, Sampler* pSampler);// for internal usage only.
		void SetTexture(const std::string& name, emberBackendInterface::ITexture* pTexture) override;
		void SetBuffer(const std::string& name, emberBackendInterface::IBuffer* pBuffer) override;
		
		// Uniform Buffer Setters:
		// Simple members:
		void SetInt(const std::string& bufferName, const std::string& memberName, int value) override;
		void SetBool(const std::string& bufferName, const std::string& memberName, bool value) override;
		void SetFloat(const std::string& bufferName, const std::string& memberName, float value) override;
		void SetFloat2(const std::string& bufferName, const std::string& memberName, const Float2& value) override;
		void SetFloat3(const std::string& bufferName, const std::string& memberName, const Float3& value) override;
		void SetFloat4(const std::string& bufferName, const std::string& memberName, const Float4& value) override;
		void SetFloat4x4(const std::string& bufferName, const std::string& memberName, const Float4x4& value) override;
		// Array members:
		void SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value) override;
		void SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value) override;
		void SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value) override;
		void SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value) override;
		void SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value) override;
		void SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value) override;
		void SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value) override;
		// Array of structs members:
		void SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value) override;
		void SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value) override;
		void SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value) override;
		void SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value) override;
		void SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value) override;
		void SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value) override;
		void SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value) override;
		// Array of arrays members:
		void SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value) override;
		void SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value) override;
		void SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value) override;
		void SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value) override;
		void SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value) override;
		void SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value) override;
		void SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value) override;

		// Uniform Buffer Getters:
		// Simple members:
		int GetInt(const std::string& bufferName, const std::string& memberName) const override;
		bool GetBool(const std::string& bufferName, const std::string& memberName) const override;
		float GetFloat(const std::string& bufferName, const std::string& memberName) const override;
		Float2 GetFloat2(const std::string& bufferName, const std::string& memberName) const override;
		Float3 GetFloat3(const std::string& bufferName, const std::string& memberName) const override;
		Float4 GetFloat4(const std::string& bufferName, const std::string& memberName) const override;
		Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& memberName) const override;
		// Array members:
		int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const override;
		bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const override;
		float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const override;
		Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const override;
		Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const override;
		Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const override;
		Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const override;
		// Array of structs members:
		int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const override;
		bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const override;
		float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const override;
		Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const override;
		Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const override;
		Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const override;
		Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const override;
		// Array of arrays members:
		int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const override;
		bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const override;
		float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const override;
		Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const override;
		Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const override;
		Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const override;
		Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const override;

		// Getters:
		Shader* GetShader() const;
		Sampler* GetSampler(const std::string& name) const;
		Texture* GetTexture(const std::string& name) const;
		
		// Backend functionality:
		void UpdateShaderData(uint32_t frameIndex = -1); // -1 = current frame index.
		const VkDescriptorSet& GetDescriptorSet(uint32_t frameIndex);
		std::vector<VkDescriptorSet>& GetDescriptorSets();

		// Debugging:
		void Print(const std::string& name) const;
		void PrintMaps() const;

	private: // Methods:
		// Initializers:
		void InitUniformBufferResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding);
		void InitSamplerResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Sampler* pSampler);
		void InitTextureResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Texture* pTexture, DescriptorType descriptorType);
		void InitBufferResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Buffer* pBuffer, DescriptorType descriptorType);
		void InitStagingMaps();
		void InitDescriptorSets();

		// Descriptor Set management:
		void CreateDescriptorSets();
		void UpdateDescriptorSet(uint32_t frameIndex, UniformBufferBinding samplerResourceBinding);
		void UpdateDescriptorSet(uint32_t frameIndex, SamplerBinding samplerResourceBinding);
		void UpdateDescriptorSet(uint32_t frameIndex, TextureBinding textureResourceBinding);
		void UpdateDescriptorSet(uint32_t frameIndex, BufferBinding bufferResourceBinding);

		// Getter templates, used in actual getters:
		template<typename T>
		T GetValue(const std::string& bufferName, const std::string& memberName) const;
		template<typename T>
		T GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		template<typename T>
		T GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		template<typename T>
		T GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;

		// Setter templates, used in actual setters:
		template<typename T>
		void SetValue(const std::string& bufferName, const std::string& memberName, const T& value);
		template<typename T>
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const T& value);
		template<typename T>
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value);
		template<typename T>
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value);
	};
}