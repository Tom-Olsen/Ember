#include "shaderProperties.h"
#include "buffer.h"
#include "iDescriptorSetBinding.h"
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
	emberBackendInterface::IDescriptorSetBinding* ShaderProperties::GetValidCallInterfaceHandle()
	{
		ValidateCallDescriptorSetBinding();
		if (m_pICallDescriptorSetBinding)
			return m_pICallDescriptorSetBinding;
		if (m_callDescriptorSetBindingExpired)
			throw std::runtime_error("ShaderProperties: call descriptor set binding has expired.");
		throw std::runtime_error("ShaderProperties: call descriptor set binding not available.");
	}



	// Constructor/Destructor:
	ShaderProperties::ShaderProperties()
	{
		m_ownsICallDescriptorSetBinding = false;
		m_callDescriptorSetBindingExpired = false;
		m_callDescriptorSetBindingGeneration = 0;
		m_pICallDescriptorSetBinding = nullptr;
	}
	ShaderProperties::ShaderProperties(emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding)
	{
		m_ownsICallDescriptorSetBinding = false;
		m_callDescriptorSetBindingExpired = false;
		m_pICallDescriptorSetBinding = pICallDescriptorSetBinding;
		m_callDescriptorSetBindingGeneration = m_pICallDescriptorSetBinding ? m_pICallDescriptorSetBinding->GetGeneration() : 0;
	}
	ShaderProperties::ShaderProperties(const Material& material)
	{
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		m_ownsICallDescriptorSetBinding = true;
		m_callDescriptorSetBindingExpired = false;
		m_pICallDescriptorSetBinding = Renderer::CreateDrawCallDescriptorSetBinding(pIMaterial);
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
		m_pICallDescriptorSetBinding = other.m_pICallDescriptorSetBinding;

		other.m_ownsICallDescriptorSetBinding = false;
		other.m_callDescriptorSetBindingExpired = false;
		other.m_callDescriptorSetBindingGeneration = 0;
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
			m_pICallDescriptorSetBinding = other.m_pICallDescriptorSetBinding;

			other.m_ownsICallDescriptorSetBinding = false;
			other.m_callDescriptorSetBindingExpired = false;
			other.m_callDescriptorSetBindingGeneration = 0;
			other.m_pICallDescriptorSetBinding = nullptr;
		}
		return *this;
	}



	// Public methods:
	// Setters:
	void ShaderProperties::SetTexture(const std::string& name, Texture& texture)
	{
		GetValidCallInterfaceHandle()->SetTexture(name, texture.GetInterfaceHandle());
	}
	void ShaderProperties::SetBuffer(const std::string& name, Buffer& buffer)
	{
		GetValidCallInterfaceHandle()->SetBuffer(name, buffer.GetInterfaceHandle());
	}



	// Uniform Buffer Setters:
	// Simple members:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, int value)
	{
		GetValidCallInterfaceHandle()->SetInt(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, bool value)
	{
		GetValidCallInterfaceHandle()->SetBool(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, float value)
	{
		GetValidCallInterfaceHandle()->SetFloat(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value)
	{
		GetValidCallInterfaceHandle()->SetFloat2(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value)
	{
		GetValidCallInterfaceHandle()->SetFloat3(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4(bufferName, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4x4(bufferName, memberName, value);
	}
	// Array members:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		GetValidCallInterfaceHandle()->SetInt(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		GetValidCallInterfaceHandle()->SetBool(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		GetValidCallInterfaceHandle()->SetFloat(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		GetValidCallInterfaceHandle()->SetFloat2(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		GetValidCallInterfaceHandle()->SetFloat3(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4x4(bufferName, arrayName, arrayIndex, value);
	}
	// Struct members inside arrays:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		GetValidCallInterfaceHandle()->SetInt(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		GetValidCallInterfaceHandle()->SetBool(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		GetValidCallInterfaceHandle()->SetFloat(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		GetValidCallInterfaceHandle()->SetFloat2(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		GetValidCallInterfaceHandle()->SetFloat3(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4x4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	// Arrays inside arrays:
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		GetValidCallInterfaceHandle()->SetInt(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		GetValidCallInterfaceHandle()->SetBool(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		GetValidCallInterfaceHandle()->SetFloat(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		GetValidCallInterfaceHandle()->SetFloat2(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		GetValidCallInterfaceHandle()->SetFloat3(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4x4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}



	// Getters:
	std::string ShaderProperties::GetShaderName() const
	{
		if (m_pICallDescriptorSetBinding)
			return m_pICallDescriptorSetBinding->GetShaderName();
		return "";
	}
	bool ShaderProperties::HasBinding(const std::string& name)
	{
		ValidateCallDescriptorSetBinding();
		if (m_pICallDescriptorSetBinding && m_pICallDescriptorSetBinding->HasBinding(name))
			return true;
		return false;
	}



	// Debugging:
	void ShaderProperties::Print() const
	{
		if (m_pICallDescriptorSetBinding)
			m_pICallDescriptorSetBinding->Print();
	}
	void ShaderProperties::PrintMaps() const
	{
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
}
