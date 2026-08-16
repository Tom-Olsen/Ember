#include "shader.h"
#include "buffer.h"
#include "iDescriptorSetBinding.h"
#include "logger.h"
#include "texture.h"
#include <stdexcept>



namespace emberCore
{
	// Protected methods:
    // Constructors/Destructor:
	Shader::Shader()
	{
		m_pIShaderDescriptorSetBinding = nullptr;
	}
	Shader::Shader(emberBackendInterface::IDescriptorSetBinding* pIShaderDescriptorSetBinding)
	{
		m_pIShaderDescriptorSetBinding = pIShaderDescriptorSetBinding;
	}
	Shader::~Shader()
    {

    }



	// Getters:
	emberBackendInterface::IDescriptorSetBinding* Shader::GetShaderDescriptorSetBinding() const
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			return pDescriptorSetBinding;
		throw std::runtime_error("Shader::GetShaderDescriptorSetBinding() failed. Shader is invalid, expired, or immutable.");
	}
	emberBackendInterface::IDescriptorSetBinding* Shader::TryGetShaderDescriptorSetBinding() const
	{
		return m_pIShaderDescriptorSetBinding;
	}



	// Public methods:
	// Movable:
	Shader::Shader(Shader&& other) noexcept
	{
		m_pIShaderDescriptorSetBinding = other.m_pIShaderDescriptorSetBinding;
		other.m_pIShaderDescriptorSetBinding = nullptr;
	}
	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this != &other)
		{
			m_pIShaderDescriptorSetBinding = other.m_pIShaderDescriptorSetBinding;
			other.m_pIShaderDescriptorSetBinding = nullptr;
		}
		return *this;
	}



	// Setters:
	void Shader::SetTexture(const std::string& name, Texture& texture)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetTexture(name, texture.GetInterfaceHandle());
		else
			LOG_WARN("Shader::SetTexture(...) failed. Shader is invalid, expired, or immutable: name='{}'.", name);
	}
	void Shader::SetBuffer(const std::string& name, Buffer& buffer)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetBuffer(name, buffer.GetInterfaceHandle());
		else
			LOG_WARN("Shader::SetBuffer(...) failed. Shader is invalid, expired, or immutable: name='{}'.", name);
	}



	// Uniform Buffer Setters:
	// Simple members:
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, int value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetInt(bufferName, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=int, bufferName='{}', memberName='{}'.", bufferName, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, bool value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetBool(bufferName, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=bool, bufferName='{}', memberName='{}'.", bufferName, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, float value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat(bufferName, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=float, bufferName='{}', memberName='{}'.", bufferName, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat2(bufferName, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float2, bufferName='{}', memberName='{}'.", bufferName, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat3(bufferName, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float3, bufferName='{}', memberName='{}'.", bufferName, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat4(bufferName, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float4, bufferName='{}', memberName='{}'.", bufferName, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat4x4(bufferName, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float4x4, bufferName='{}', memberName='{}'.", bufferName, memberName);
	}
	// Array members:
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetInt(bufferName, arrayName, arrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=int, bufferName='{}', arrayName='{}', arrayIndex={}.", bufferName, arrayName, arrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetBool(bufferName, arrayName, arrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=bool, bufferName='{}', arrayName='{}', arrayIndex={}.", bufferName, arrayName, arrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat(bufferName, arrayName, arrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=float, bufferName='{}', arrayName='{}', arrayIndex={}.", bufferName, arrayName, arrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat2(bufferName, arrayName, arrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float2, bufferName='{}', arrayName='{}', arrayIndex={}.", bufferName, arrayName, arrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat3(bufferName, arrayName, arrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float3, bufferName='{}', arrayName='{}', arrayIndex={}.", bufferName, arrayName, arrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat4(bufferName, arrayName, arrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float4, bufferName='{}', arrayName='{}', arrayIndex={}.", bufferName, arrayName, arrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat4x4(bufferName, arrayName, arrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float4x4, bufferName='{}', arrayName='{}', arrayIndex={}.", bufferName, arrayName, arrayIndex);
	}
	// Struct members inside arrays:
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetInt(bufferName, arrayName, arrayIndex, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=int, bufferName='{}', arrayName='{}', arrayIndex={}, memberName='{}'.", bufferName, arrayName, arrayIndex, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetBool(bufferName, arrayName, arrayIndex, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=bool, bufferName='{}', arrayName='{}', arrayIndex={}, memberName='{}'.", bufferName, arrayName, arrayIndex, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat(bufferName, arrayName, arrayIndex, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=float, bufferName='{}', arrayName='{}', arrayIndex={}, memberName='{}'.", bufferName, arrayName, arrayIndex, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat2(bufferName, arrayName, arrayIndex, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float2, bufferName='{}', arrayName='{}', arrayIndex={}, memberName='{}'.", bufferName, arrayName, arrayIndex, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat3(bufferName, arrayName, arrayIndex, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float3, bufferName='{}', arrayName='{}', arrayIndex={}, memberName='{}'.", bufferName, arrayName, arrayIndex, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat4(bufferName, arrayName, arrayIndex, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float4, bufferName='{}', arrayName='{}', arrayIndex={}, memberName='{}'.", bufferName, arrayName, arrayIndex, memberName);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat4x4(bufferName, arrayName, arrayIndex, memberName, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float4x4, bufferName='{}', arrayName='{}', arrayIndex={}, memberName='{}'.", bufferName, arrayName, arrayIndex, memberName);
	}
	// Arrays inside arrays:
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetInt(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=int, bufferName='{}', arrayName='{}', arrayIndex={}, subArrayName='{}', subArrayIndex={}.", bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetBool(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=bool, bufferName='{}', arrayName='{}', arrayIndex={}, subArrayName='{}', subArrayIndex={}.", bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=float, bufferName='{}', arrayName='{}', arrayIndex={}, subArrayName='{}', subArrayIndex={}.", bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat2(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float2, bufferName='{}', arrayName='{}', arrayIndex={}, subArrayName='{}', subArrayIndex={}.", bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat3(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float3, bufferName='{}', arrayName='{}', arrayIndex={}, subArrayName='{}', subArrayIndex={}.", bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float4, bufferName='{}', arrayName='{}', arrayIndex={}, subArrayName='{}', subArrayIndex={}.", bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		if (emberBackendInterface::IDescriptorSetBinding* pDescriptorSetBinding = TryGetShaderDescriptorSetBinding())
			pDescriptorSetBinding->SetFloat4x4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
		else
			LOG_WARN("Shader::SetValue(...) failed. Shader is invalid, expired, or immutable: type=Float4x4, bufferName='{}', arrayName='{}', arrayIndex={}, subArrayName='{}', subArrayIndex={}.", bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
}