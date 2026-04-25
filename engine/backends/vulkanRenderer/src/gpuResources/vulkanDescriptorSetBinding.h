#pragma once
#include "iDescriptorSetBinding.h"
#include "vulkanDescriptorType.h"
#include "vulkanRendererExport.h"
#include "vulkanUniformBuffer.h"
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
namespace emberBufferLayout
{
	struct BufferLayout;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Shader;
	class Buffer;
	class Texture;



	// Resource bindings:
	struct UniformBufferBinding	// UniformBuffer is a Buffer, but gets handled differently than other buffers as it owns cpu side memory which must be uploaded to gpu on update.
	{
		uint32_t binding;
		UniformBuffer uniformBuffer;
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
	/// Each DescriptorSetBinding links to one specific vkDescriptorSet and manages all the associated gpu resources.
	/// DescriptorSetBinding owns UniformBuffer; Buffers and Texture pointers are not owned by DescriptorSetBinding.
	/// DescriptorSetBinding construction is expensive, pre create them or use the DescriptorSetBindingPool.
	/// </summary>
	class VULKAN_RENDERER_API DescriptorSetBinding : public emberBackendInterface::IDescriptorSetBinding
	{
	private: // Members:
		Shader* m_pShader;
		uint32_t m_setIndex;
		uint64_t m_generation;

        // Fast lookup of indices by name and vice versa:
		std::unordered_map<std::string, uint32_t> m_bindingIndices;
		std::unordered_map<uint32_t, std::string> m_bindingNames;   // for debug output only.

		// All these vectors contain one item for each frame in flight:
		std::vector<VkDescriptorSet> m_descriptorSets;
		std::vector<std::unordered_map<uint32_t, TextureBinding>> m_textureMaps;
		std::vector<std::unordered_map<uint32_t, BufferBinding>> m_bufferMaps;
		std::unordered_map<uint32_t, UniformBufferBinding> m_uniformBufferMap; // uniformBuffer handles framesInFlight with a ring buffer.

		// UniformBuffer does not need stagingMap, as it contains a host and device buffer, where the host buffer acts as a staging buffer:
		std::unordered_map<uint32_t, Texture*> m_textureStagingMap;
		std::unordered_map<uint32_t, Buffer*> m_bufferStagingMap;

	public: // Methods:
		// Constructors/Destructor:
		DescriptorSetBinding(Shader* pShader, uint32_t setIndex);
		~DescriptorSetBinding();

		// Non-copyable:
		DescriptorSetBinding(const DescriptorSetBinding& other) = delete;
		DescriptorSetBinding& operator=(const DescriptorSetBinding& other) = delete;

		// Movable:
		DescriptorSetBinding(DescriptorSetBinding&& other) noexcept;
		DescriptorSetBinding& operator=(DescriptorSetBinding&& other) noexcept;

		// Setters:
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
		uint32_t GetSetIndex() const;
		uint64_t GetGeneration() const override;
		bool HasBinding(const std::string& name) const override;
		std::string GetShaderName() const override;
		Texture* GetTexture(const std::string& name) const;
		
		// Backend functionality:
		void InvalidateBorrowedHandles();
		void UpdateShaderData(uint32_t frameIndex);
		const VkDescriptorSet& GetVkDescriptorSet(uint32_t frameIndex) const;
		const std::vector<VkDescriptorSet>& GetVkDescriptorSets() const;

		// Debugging:
		void Print() const override;
		void PrintMaps() const override;

	private: // Methods:
		const uint32_t* FindBindingIndex(const std::string& name) const;

		// Initializers:
		void InitUniformBufferBinding(uint32_t binding, const emberBufferLayout::BufferLayout& bufferLayout);
		void InitTextureBinding(uint32_t frameIndex, uint32_t binding, Texture* pTexture, DescriptorType descriptorType);
		void InitBufferBinding(uint32_t frameIndex, uint32_t binding, Buffer* pBuffer, DescriptorType descriptorType);
		void InitStagingMaps();
		void InitDescriptorSets();

		// Descriptor Set management:
		void CreateDescriptorSets();
		void UpdateDescriptorSet(uint32_t frameIndex, const UniformBufferBinding& uniformBufferBinding);
		void UpdateDescriptorSet(uint32_t frameIndex, const TextureBinding& textureBinding);
		void UpdateDescriptorSet(uint32_t frameIndex, const BufferBinding& buffeBinding);

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