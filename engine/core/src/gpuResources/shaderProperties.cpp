#include "shaderProperties.h"
#include "buffer.h"
#include "computeShader.h"
#include "iDescriptorSetBinding.h"
#include "iComputeShader.h"
#include "iMaterial.h"
#include "iMesh.h"
#include "logger.h"
#include "material.h"
#include "renderer.h"
#include "texture.h"
#include <stdexcept>



namespace emberEngine
{
	emberBackendInterface::IDescriptorSetBinding* ShaderProperties::GetCallInterfaceHandle()
	{
		ValidateCallDescriptorSetBinding();
		return m_pICallDescriptorSetBinding;
	}



	// Constructor/Destructor:
	ShaderProperties::ShaderProperties()
	{
		m_ownsICallDescriptorSetBinding = false;
		m_callDescriptorSetBindingExpired = false;
		m_callDescriptorSetBindingGeneration = 0;
		m_pIShaderDescriptorSetBinding = nullptr;
		m_pICallDescriptorSetBinding = nullptr;
	}
	ShaderProperties::ShaderProperties(ComputeShader& computeShader)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		m_ownsICallDescriptorSetBinding = true;
		m_callDescriptorSetBindingExpired = false;
		m_pIShaderDescriptorSetBinding = pIComputeShader->GetShaderDescriptorSetBinding();
		m_pICallDescriptorSetBinding = Renderer::CreateComputeCallDescriptorSetBinding(pIComputeShader);
		m_callDescriptorSetBindingGeneration = m_pICallDescriptorSetBinding ? m_pICallDescriptorSetBinding->GetGeneration() : 0;
	}
	ShaderProperties::ShaderProperties(ComputeShader& computeShader, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, bool ownsCallDescriptorSetBinding)
	{
		emberBackendInterface::IComputeShader* pIComputeShader = computeShader.GetInterfaceHandle();
		m_ownsICallDescriptorSetBinding = ownsCallDescriptorSetBinding;
		m_callDescriptorSetBindingExpired = false;
		m_pIShaderDescriptorSetBinding = pIComputeShader->GetShaderDescriptorSetBinding();
		m_pICallDescriptorSetBinding = pICallDescriptorSetBinding;
		m_callDescriptorSetBindingGeneration = m_pICallDescriptorSetBinding ? m_pICallDescriptorSetBinding->GetGeneration() : 0;
	}
	ShaderProperties::ShaderProperties(const Material& material)
	{
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		m_ownsICallDescriptorSetBinding = true;
		m_callDescriptorSetBindingExpired = false;
		m_pIShaderDescriptorSetBinding = pIMaterial->GetShaderDescriptorSetBinding();
		m_pICallDescriptorSetBinding = Renderer::CreateDrawCallDescriptorSetBinding(pIMaterial);
		m_callDescriptorSetBindingGeneration = m_pICallDescriptorSetBinding ? m_pICallDescriptorSetBinding->GetGeneration() : 0;
	}
	ShaderProperties::ShaderProperties(const Material& material, emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding, bool ownsCallDescriptorSetBinding)
	{
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		m_ownsICallDescriptorSetBinding = ownsCallDescriptorSetBinding;
		m_callDescriptorSetBindingExpired = false;
		m_pIShaderDescriptorSetBinding = pIMaterial->GetShaderDescriptorSetBinding();
		m_pICallDescriptorSetBinding = pICallDescriptorSetBinding;
		m_callDescriptorSetBindingGeneration = m_pICallDescriptorSetBinding ? m_pICallDescriptorSetBinding->GetGeneration() : 0;
	}
	ShaderProperties::~ShaderProperties()
	{
		if (m_ownsICallDescriptorSetBinding && m_pICallDescriptorSetBinding)
			delete m_pICallDescriptorSetBinding;
	}



	// Movable:
	ShaderProperties::ShaderProperties(ShaderProperties&& other) noexcept
	{
		m_ownsICallDescriptorSetBinding = other.m_ownsICallDescriptorSetBinding;
		m_callDescriptorSetBindingExpired = other.m_callDescriptorSetBindingExpired;
		m_callDescriptorSetBindingGeneration = other.m_callDescriptorSetBindingGeneration;
		m_pIShaderDescriptorSetBinding = other.m_pIShaderDescriptorSetBinding;
		m_pICallDescriptorSetBinding = other.m_pICallDescriptorSetBinding;

		other.m_ownsICallDescriptorSetBinding = false;
		other.m_callDescriptorSetBindingExpired = false;
		other.m_callDescriptorSetBindingGeneration = 0;
		other.m_pIShaderDescriptorSetBinding = nullptr;
		other.m_pICallDescriptorSetBinding = nullptr;
	}
	ShaderProperties& ShaderProperties::operator=(ShaderProperties&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ownsICallDescriptorSetBinding)
				delete m_pICallDescriptorSetBinding;

			m_ownsICallDescriptorSetBinding = other.m_ownsICallDescriptorSetBinding;
			m_callDescriptorSetBindingExpired = other.m_callDescriptorSetBindingExpired;
			m_callDescriptorSetBindingGeneration = other.m_callDescriptorSetBindingGeneration;
			m_pIShaderDescriptorSetBinding = other.m_pIShaderDescriptorSetBinding;
			m_pICallDescriptorSetBinding = other.m_pICallDescriptorSetBinding;

			other.m_ownsICallDescriptorSetBinding = false;
			other.m_callDescriptorSetBindingExpired = false;
			other.m_callDescriptorSetBindingGeneration = 0;
			other.m_pIShaderDescriptorSetBinding = nullptr;
			other.m_pICallDescriptorSetBinding = nullptr;
		}
		return *this;
	}



	// Public methods:
	// Setters:
	void ShaderProperties::SetTexture(const std::string& name, Texture& texture)
	{
		GetDescriptorSetBindingWith(name)->SetTexture(name, texture.GetInterfaceHandle());
	}
	void ShaderProperties::SetBuffer(const std::string& name, Buffer& buffer)
	{
		GetDescriptorSetBindingWith(name)->SetBuffer(name, buffer.GetInterfaceHandle());
	}



	// Uniform Buffer Setters:
	// Simple members:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, int value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetInt(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, bool value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetBool(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, float value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat2(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat3(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat4(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat4x4(bufferName, memberName, value);
	}
	// Array members:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetInt(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetBool(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat2(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat3(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat4(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat4x4(bufferName, arrayName, arrayIndex, value);
	}
	// Struct members inside arrays:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetInt(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetBool(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat2(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat3(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat4x4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	// Arrays inside arrays:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetInt(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetBool(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat2(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat3(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		GetDescriptorSetBindingWith(bufferName)->SetFloat4x4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}



	// Getters:
	std::string ShaderProperties::GetShaderName() const
	{
		if (m_pIShaderDescriptorSetBinding)
			return m_pIShaderDescriptorSetBinding->GetShaderName();
		if (m_pICallDescriptorSetBinding)
			return m_pICallDescriptorSetBinding->GetShaderName();
		return "";
	}



	// Debugging:
	void ShaderProperties::Print() const
	{
		if (m_pIShaderDescriptorSetBinding)
			m_pIShaderDescriptorSetBinding->Print();
		if (m_pICallDescriptorSetBinding)
			m_pICallDescriptorSetBinding->Print();
	}
	void ShaderProperties::PrintMaps() const
	{
		if (m_pIShaderDescriptorSetBinding)
			m_pIShaderDescriptorSetBinding->PrintMaps();
		if (m_pICallDescriptorSetBinding)
			m_pICallDescriptorSetBinding->PrintMaps();
	}



	// Private methods:
	void ShaderProperties::ValidateCallDescriptorSetBinding()
	{
		if (!m_pICallDescriptorSetBinding)
			return;
		if (m_pICallDescriptorSetBinding->GetGeneration() == m_callDescriptorSetBindingGeneration)
			return;

		LOG_WARN("ShaderProperties for shader '{}' points to an expired call descriptor set binding. Ignoring stale call-local properties until this ShaderProperties is reassigned.", GetShaderName());
		m_pICallDescriptorSetBinding = nullptr;
		m_ownsICallDescriptorSetBinding = false;
		m_callDescriptorSetBindingExpired = true;
		m_callDescriptorSetBindingGeneration = 0;
	}
	emberBackendInterface::IDescriptorSetBinding* ShaderProperties::GetDescriptorSetBindingWith(const std::string& name)
	{
		ValidateCallDescriptorSetBinding();
		if (m_pIShaderDescriptorSetBinding && m_pIShaderDescriptorSetBinding->HasBinding(name))
			return m_pIShaderDescriptorSetBinding;
		if (m_pICallDescriptorSetBinding && m_pICallDescriptorSetBinding->HasBinding(name))
			return m_pICallDescriptorSetBinding;
		if (m_callDescriptorSetBindingExpired && m_pIShaderDescriptorSetBinding)
		{
			LOG_WARN("ShaderProperties ignored descriptor '{}' because its call descriptor set binding has expired.", name);
			return m_pIShaderDescriptorSetBinding;
		}
		throw std::runtime_error("ShaderProperties: descriptor binding not found: " + name);
	}
}