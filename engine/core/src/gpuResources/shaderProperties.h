#pragma once
#include "emberMath.h"
#include <memory>



// Forward decleration:
namespace emberBackendInterface
{
	class IShaderProperties;
}



namespace emberEngine
{
	// Forward decleration:
	class ComputeShader;
	class Material;
	class StorageBuffer;
	class Texture2d;



	class ShaderProperties
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::IShaderProperties> m_pIShaderProperties;

	public: // Constructors/Destructor:
		ShaderProperties(const ComputeShader* pComputeShader);
		ShaderProperties(const Material* pMaterial);
		~ShaderProperties();

	public: // Methods:
		// Setters:
		void SetTexture2d(const std::string& name, Texture2d* pTexture2d);
		void SetStorageBuffer(const std::string& name, StorageBuffer* pStorageBuffer);

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