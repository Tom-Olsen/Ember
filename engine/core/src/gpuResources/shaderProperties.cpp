#include "shaderProperties.h"
#include "buffer.h"
#include "computeShader.h"
#include "material.h"
#include "texture.h"
#include "vulkanComputeShader.h"
#include "vulkanMaterial.h"
#include "vulkanShader.h"
#include "vulkanShaderProperties.h"



namespace emberEngine
{
	emberBackendInterface::IShaderProperties* ShaderProperties::GetInterfaceHandle()
	{
		return m_pIShaderProperties.get();
	}



	// Constructor/Destructor:
	ShaderProperties::ShaderProperties()
	{
		m_pIShaderProperties = nullptr;
	}
	ShaderProperties::ShaderProperties(ComputeShader& computeShader)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		vulkanRendererBackend::ComputeShader* pComputeShader = static_cast<vulkanRendererBackend::ComputeShader*>(pIComputeShader);
		vulkanRendererBackend::ComputeShader* pShader = static_cast<vulkanRendererBackend::ComputeShader*>(pComputeShader);
		m_pIShaderProperties = std::make_unique<vulkanRendererBackend::ShaderProperties>(pShader);
	}
	ShaderProperties::ShaderProperties(Material& material)
	{
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		vulkanRendererBackend::Material* pMaterial = static_cast<vulkanRendererBackend::Material*>(pIMaterial);
		vulkanRendererBackend::Shader* pShader = static_cast<vulkanRendererBackend::Shader*>(pMaterial);
		m_pIShaderProperties = std::make_unique<vulkanRendererBackend::ShaderProperties>(pShader);
	}
	ShaderProperties::ShaderProperties(emberBackendInterface::IShaderProperties* pIShaderProperties)
	{
		m_pIShaderProperties = std::unique_ptr<emberBackendInterface::IShaderProperties>(pIShaderProperties);
	}
	ShaderProperties::~ShaderProperties()
	{
		m_pIShaderProperties.release();
	}



	// Movable:
	ShaderProperties::ShaderProperties(ShaderProperties&& other) noexcept = default;
	ShaderProperties& ShaderProperties::operator=(ShaderProperties&& other) noexcept = default;



	// Public methods:
	// Setters:
	void ShaderProperties::SetTexture(const std::string& name, Texture& texture)
	{
		m_pIShaderProperties->SetTexture(name, texture.GetInterfaceHandle());
	}
	void ShaderProperties::SetBuffer(const std::string& name, Buffer& buffer)
	{
		m_pIShaderProperties->SetBuffer(name, buffer.GetInterfaceHandle());
	}



	// Uniform Buffer Setters:
	// Simple members:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, int value)
	{
		m_pIShaderProperties->SetInt(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, bool value)
	{
		m_pIShaderProperties->SetBool(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, float value)
	{
		m_pIShaderProperties->SetFloat(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value)
	{
		m_pIShaderProperties->SetFloat2(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value)
	{
		m_pIShaderProperties->SetFloat3(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value)
	{
		m_pIShaderProperties->SetFloat4(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value)
	{
		m_pIShaderProperties->SetFloat4x4(bufferName, memberName, value);
	}
	// Array members:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		m_pIShaderProperties->SetInt(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		m_pIShaderProperties->SetBool(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		m_pIShaderProperties->SetFloat(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		m_pIShaderProperties->SetFloat2(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		m_pIShaderProperties->SetFloat3(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		m_pIShaderProperties->SetFloat4(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		m_pIShaderProperties->SetFloat4x4(bufferName, arrayName, arrayIndex, value);
	}
	// Struct members inside arrays:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		m_pIShaderProperties->SetInt(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		m_pIShaderProperties->SetBool(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		m_pIShaderProperties->SetFloat(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		m_pIShaderProperties->SetFloat2(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		m_pIShaderProperties->SetFloat3(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		m_pIShaderProperties->SetFloat4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		m_pIShaderProperties->SetFloat4x4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	// Arrays inside arrays:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		m_pIShaderProperties->SetInt(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		m_pIShaderProperties->SetBool(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		m_pIShaderProperties->SetFloat(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		m_pIShaderProperties->SetFloat2(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		m_pIShaderProperties->SetFloat3(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		m_pIShaderProperties->SetFloat4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		m_pIShaderProperties->SetFloat4x4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
}