#include "callProperties.h"
#include "buffer.h"
#include "commonMaterialPass.h"
#include "iDescriptorSetBinding.h"
#include "logger.h"
#include "material.h"
#include "renderer.h"
#include "shadowMaterial.h"
#include "texture.h"
#include <stdexcept>
#include <utility>



namespace emberCore
{

	// Public methods:
	// Constructor/Destructor:
	CallProperties::CallProperties()
	{
		m_ownsICallDescriptorSetBinding = false;
		m_callDescriptorSetBindingExpired = false;
		m_callDescriptorSetBindingGeneration = 0;
		m_pICallDescriptorSetBinding = nullptr;
	}
	CallProperties::CallProperties(const Material& material)
	{
		emberBackendInterface::IMaterial* pIMaterial = material.TryGetInterfaceHandle();
		m_ownsICallDescriptorSetBinding = true;
		m_callDescriptorSetBindingExpired = false;
		m_pICallDescriptorSetBinding = Renderer::CreateDrawCallDescriptorSetBinding(pIMaterial);
		m_callDescriptorSetBindingGeneration = m_pICallDescriptorSetBinding ? m_pICallDescriptorSetBinding->GetGeneration() : 0;

		// Link shadow callProperties:
		emberCommon::MaterialPass materialPass = material.GetMaterialPass();
		if (emberCommon::IsSurfaceMaterialPass(materialPass))
		{
			ShadowMaterial shadowMaterial = material.GetShadowMaterial();
			if (shadowMaterial.IsValid())
				m_pShadowProperties = std::make_unique<CallProperties>(shadowMaterial);
		}
	}
	CallProperties::~CallProperties()
	{
		if (m_ownsICallDescriptorSetBinding && m_pICallDescriptorSetBinding)
			delete m_pICallDescriptorSetBinding;
	}



	// Movable:
	CallProperties::CallProperties(CallProperties&& other) noexcept
	{
		m_ownsICallDescriptorSetBinding = other.m_ownsICallDescriptorSetBinding;
		m_callDescriptorSetBindingExpired = other.m_callDescriptorSetBindingExpired;
		m_callDescriptorSetBindingGeneration = other.m_callDescriptorSetBindingGeneration;
		m_pICallDescriptorSetBinding = other.m_pICallDescriptorSetBinding;
		m_pShadowProperties = std::move(other.m_pShadowProperties);

		other.m_ownsICallDescriptorSetBinding = false;
		other.m_callDescriptorSetBindingExpired = false;
		other.m_callDescriptorSetBindingGeneration = 0;
		other.m_pICallDescriptorSetBinding = nullptr;
	}
	CallProperties& CallProperties::operator=(CallProperties&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ownsICallDescriptorSetBinding)
				delete m_pICallDescriptorSetBinding;

			m_ownsICallDescriptorSetBinding = other.m_ownsICallDescriptorSetBinding;
			m_callDescriptorSetBindingExpired = other.m_callDescriptorSetBindingExpired;
			m_callDescriptorSetBindingGeneration = other.m_callDescriptorSetBindingGeneration;
			m_pICallDescriptorSetBinding = other.m_pICallDescriptorSetBinding;
			m_pShadowProperties = std::move(other.m_pShadowProperties);

			other.m_ownsICallDescriptorSetBinding = false;
			other.m_callDescriptorSetBindingExpired = false;
			other.m_callDescriptorSetBindingGeneration = 0;
			other.m_pICallDescriptorSetBinding = nullptr;
		}
		return *this;
	}



	// Setters:
	void CallProperties::SetTexture(const std::string& name, Texture& texture)
	{
		GetValidCallInterfaceHandle()->SetTexture(name, texture.GetInterfaceHandle());
	}
	void CallProperties::SetBuffer(const std::string& name, Buffer& buffer)
	{
		GetValidCallInterfaceHandle()->SetBuffer(name, buffer.GetInterfaceHandle());
	}



	// Uniform Buffer Setters:
	// Simple members:
	void CallProperties::SetValue(const std::string& bufferName, const std::string& memberName, int value)
	{
		GetValidCallInterfaceHandle()->SetInt(bufferName, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& memberName, bool value)
	{
		GetValidCallInterfaceHandle()->SetBool(bufferName, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& memberName, float value)
	{
		GetValidCallInterfaceHandle()->SetFloat(bufferName, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value)
	{
		GetValidCallInterfaceHandle()->SetFloat2(bufferName, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value)
	{
		GetValidCallInterfaceHandle()->SetFloat3(bufferName, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4(bufferName, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4x4(bufferName, memberName, value);
	}
	// Array members:
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		GetValidCallInterfaceHandle()->SetInt(bufferName, arrayName, arrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		GetValidCallInterfaceHandle()->SetBool(bufferName, arrayName, arrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		GetValidCallInterfaceHandle()->SetFloat(bufferName, arrayName, arrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		GetValidCallInterfaceHandle()->SetFloat2(bufferName, arrayName, arrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		GetValidCallInterfaceHandle()->SetFloat3(bufferName, arrayName, arrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4(bufferName, arrayName, arrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4x4(bufferName, arrayName, arrayIndex, value);
	}
	// Struct members inside arrays:
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		GetValidCallInterfaceHandle()->SetInt(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		GetValidCallInterfaceHandle()->SetBool(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		GetValidCallInterfaceHandle()->SetFloat(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		GetValidCallInterfaceHandle()->SetFloat2(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		GetValidCallInterfaceHandle()->SetFloat3(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4x4(bufferName, arrayName, arrayIndex, memberName, value);
	}
	// Arrays inside arrays:
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		GetValidCallInterfaceHandle()->SetInt(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		GetValidCallInterfaceHandle()->SetBool(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		GetValidCallInterfaceHandle()->SetFloat(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		GetValidCallInterfaceHandle()->SetFloat2(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		GetValidCallInterfaceHandle()->SetFloat3(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void CallProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		GetValidCallInterfaceHandle()->SetFloat4x4(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}



	// Getters:
	bool CallProperties::HasBinding(const std::string& name)
	{
		ValidateCallDescriptorSetBinding();
		if (m_pICallDescriptorSetBinding && m_pICallDescriptorSetBinding->HasBinding(name))
			return true;
		return false;
	}
	bool CallProperties::HasShadowProperties()
	{
		return m_pShadowProperties != nullptr && m_pShadowProperties->GetCallInterfaceHandle() != nullptr;
	}
	CallProperties& CallProperties::GetShadowProperties()
	{
		if (!HasShadowProperties())
			throw std::runtime_error("CallProperties::GetShadowProperties() failed. Shadow properties are not available.");
		return *m_pShadowProperties;
	}



	// Debugging:
	void CallProperties::Print() const
	{
		if (m_pICallDescriptorSetBinding)
			m_pICallDescriptorSetBinding->Print();
	}
	void CallProperties::PrintMaps() const
	{
		if (m_pICallDescriptorSetBinding)
			m_pICallDescriptorSetBinding->PrintMaps();
	}



	// Private methods:
	CallProperties::CallProperties(emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding)
	{
		m_ownsICallDescriptorSetBinding = false;
		m_callDescriptorSetBindingExpired = false;
		m_pICallDescriptorSetBinding = pICallDescriptorSetBinding;
		m_callDescriptorSetBindingGeneration = m_pICallDescriptorSetBinding ? m_pICallDescriptorSetBinding->GetGeneration() : 0;
	}
	void CallProperties::ValidateCallDescriptorSetBinding()
	{
		if (!m_pICallDescriptorSetBinding)
			return;
		if (m_pICallDescriptorSetBinding->GetGeneration() == m_callDescriptorSetBindingGeneration)
			return;

		LOG_WARN("CallProperties points to an expired call descriptor set binding. Ignoring stale call-local properties until this CallProperties is reassigned.");
		m_pICallDescriptorSetBinding = nullptr;
		m_ownsICallDescriptorSetBinding = false;
		m_callDescriptorSetBindingExpired = true;
		m_callDescriptorSetBindingGeneration = 0;
	}
	void CallProperties::SetShadowProperties(CallProperties&& shadowProperties)
	{
		if (shadowProperties.GetCallInterfaceHandle() == nullptr)
		{
			m_pShadowProperties.reset();
			return;
		}
		m_pShadowProperties = std::make_unique<CallProperties>(std::move(shadowProperties));
	}
	emberBackendInterface::IDescriptorSetBinding* CallProperties::GetCallInterfaceHandle()
	{
		ValidateCallDescriptorSetBinding();
		return m_pICallDescriptorSetBinding;
	}
	emberBackendInterface::IDescriptorSetBinding* CallProperties::GetValidCallInterfaceHandle()
	{
		ValidateCallDescriptorSetBinding();
		if (m_pICallDescriptorSetBinding)
			return m_pICallDescriptorSetBinding;
		if (m_callDescriptorSetBindingExpired)
			throw std::runtime_error("CallProperties: call descriptor set binding has expired.");
		throw std::runtime_error("CallProperties: call descriptor set binding not available.");
	}
}