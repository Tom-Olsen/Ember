#include "shader.h"
#include "buffer.h"
#include "iDescriptorSetBinding.h"
#include "texture.h"



namespace emberEngine
{
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
		m_pIShaderDescriptorSetBinding->SetTexture(name, texture.GetInterfaceHandle());
	}
	void Shader::SetBuffer(const std::string& name, Buffer& buffer)
	{
		m_pIShaderDescriptorSetBinding->SetBuffer(name, buffer.GetInterfaceHandle());
	}



	// Uniform Buffer Setters:
	// Simple members:
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, int value)
	{
		m_pIShaderDescriptorSetBinding->SetInt(bufferName, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, bool value)
	{
		m_pIShaderDescriptorSetBinding->SetBool(bufferName, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, float value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat(bufferName, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat2(bufferName, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat3(bufferName, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat4(bufferName, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat4x4(bufferName, memberName, value);
	}
	// Array members:
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		m_pIShaderDescriptorSetBinding->SetInt(bufferName, arrayName, arrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		m_pIShaderDescriptorSetBinding->SetBool(bufferName, arrayName, arrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat(bufferName, arrayName, arrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat2(bufferName, arrayName, arrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat3(bufferName, arrayName, arrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat4(bufferName, arrayName, arrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat4x4(bufferName, arrayName, arrayIndex, value);
	}
	// Struct members inside arrays:
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		m_pIShaderDescriptorSetBinding->SetInt(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		m_pIShaderDescriptorSetBinding->SetBool(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat2(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat3(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat4x4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	// Arrays inside arrays:
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		m_pIShaderDescriptorSetBinding->SetInt(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		m_pIShaderDescriptorSetBinding->SetBool(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat2(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat3(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void Shader::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		m_pIShaderDescriptorSetBinding->SetFloat4x4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
}