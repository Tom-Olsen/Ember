#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"
#include <cstdint>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IDescriptorSetBinding;
}



namespace emberCore
{
	// Forward decleration:
	class Buffer;
	class Texture;



	class EMBER_CORE_API Shader
	{
	protected: // Members:
		emberBackendInterface::IDescriptorSetBinding* m_pIShaderDescriptorSetBinding;

	protected: // Methods:
		// Constructors/Destructor:
		Shader();
		Shader(emberBackendInterface::IDescriptorSetBinding* pIShaderDescriptorSetBinding);
		~Shader();

	public: // Methods:
		// Copyable:
		Shader(const Shader&) = default;
		Shader& operator=(const Shader&) = default;

		// Movable:
		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

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
	};
}