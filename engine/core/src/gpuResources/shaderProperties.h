#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"
#include <cstdint>
#include <memory>



// Forward decleration:
namespace emberBackendInterface
{
	class IDescriptorSetBinding;
}



namespace emberEngine
{
	// Forward decleration:
	class Buffer;
	class Compute;
	class ComputeShader;
	class Material;
	class StorageBuffer;
	class Texture;



	class EMBER_CORE_API ShaderProperties
	{
		// Friends:
		friend class Renderer;
		friend class Compute;

	private: // Members:
		bool m_ownsICallDescriptorSetBinding;
		bool m_callDescriptorSetBindingExpired;
		uint64_t m_callDescriptorSetBindingGeneration;
		emberBackendInterface::IDescriptorSetBinding* m_pIShaderDescriptorSetBinding; // Owned by the shader/material/compute shader.
		emberBackendInterface::IDescriptorSetBinding* m_pICallDescriptorSetBinding; // conditional ownership, depending on usecase.
		emberBackendInterface::IDescriptorSetBinding* GetCallInterfaceHandle();

	public: // Methods:
		// Constructors/Destructor:
		ShaderProperties();
		ShaderProperties(ComputeShader& computeShader);
		ShaderProperties(ComputeShader& computeShader, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, bool ownsCallDescriptorSetBinding);
		ShaderProperties(const Material& material);
		ShaderProperties(const Material& material, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, bool ownsCallDescriptorSetBinding);
		~ShaderProperties();

		// Non-copyable:
		ShaderProperties(const ShaderProperties&) = delete;
		ShaderProperties& operator=(const ShaderProperties&) = delete;

		// Movable:
		ShaderProperties(ShaderProperties&& other) noexcept;
		ShaderProperties& operator=(ShaderProperties&& other) noexcept;

		// Setters:
		void SetTexture(const std::string& name, Texture& texture);
		void SetBuffer(const std::string& name, Buffer& buffer);

		// Uniform Buffer Setters:
		// Simple members:
		void SetValue(const std::string& bufferName, const std::string& memberName, int value);
		void SetValue(const std::string& bufferName, const std::string& memberName, bool value);
		void SetValue(const std::string& bufferName, const std::string& memberName, float value);
		void SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value);
		void SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value);
		void SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value);
		void SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value);
		// Array members:
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value);
		// Struct members inside arrays:
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value);
		// Arrays inside arrays:
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value);

		// Getters
		std::string GetShaderName() const;

		// Debugging:
		void Print() const;
		void PrintMaps() const;

	private: // Methods:
		void ValidateCallDescriptorSetBinding();
		emberBackendInterface::IDescriptorSetBinding* GetDescriptorSetBindingWith(const std::string& name);
	};
}