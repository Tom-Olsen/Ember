#include "material.h"
#include "forwardMaterial.h"
#include "gizmoMaterial.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"
#include "materialShader.h"
#include "shadowMaterial.h"
#include <stdexcept>



namespace emberCore
{
	// Protected methods:
	Material::Material(MaterialId materialId)
		: Shader()
	{
		m_materialId = materialId;
	}
	emberBackendInterface::IDescriptorSetBinding* Material::GetShaderDescriptorSetBinding() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Material::GetShaderDescriptorSetBinding() failed. Material is invalid or expired.");
		return pIMaterial->GetShaderDescriptorSetBinding();
	}



	// Public methods:
	// Constructor/Destructor:
	Material::Material()
	{
		m_materialId = invalidMaterialId;
	}
	Material::~Material()
	{

	}



	// Creation/Destruction: (register/delete from MaterialManager)
	Material Material::CreateOutline(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return MaterialManager::CreateOutlineMaterial(vertexSpv, fragmentSpv, debugName);
	}
	Material Material::CreateOutline(const MaterialShader& materialShader, const std::string& debugName)
	{
		return MaterialManager::CreateOutlineMaterial(materialShader, debugName);
	}
	ForwardMaterial Material::CreateForward(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return MaterialManager::CreateForwardMaterial(renderMode, vertexSpv, fragmentSpv, debugName);
	}
	ForwardMaterial Material::CreateForward(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& debugName)
	{
		return MaterialManager::CreateForwardMaterial(renderMode, materialShader, debugName);
	}
	GizmoMaterial Material::CreateGizmo(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return MaterialManager::CreateGizmoMaterial(renderMode, vertexSpv, fragmentSpv, debugName);
	}
	GizmoMaterial Material::CreateGizmo(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& debugName)
	{
		return MaterialManager::CreateGizmoMaterial(renderMode, materialShader, debugName);
	}
	ShadowMaterial Material::CreateShadow(const std::filesystem::path& vertexSpv, const std::string& debugName)
	{
		return MaterialManager::CreateShadowMaterial(vertexSpv, debugName);
	}
	ShadowMaterial Material::CreateShadow(const MaterialShader& materialShader, const std::string& debugName)
	{
		return MaterialManager::CreateShadowMaterial(materialShader, debugName);
	}
	Material Material::CreatePresent(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return MaterialManager::CreatePresentMaterial(vertexSpv, fragmentSpv, debugName);
	}
	Material Material::CreatePresent(const MaterialShader& materialShader, const std::string& debugName)
	{
		return MaterialManager::CreatePresentMaterial(materialShader, debugName);
	}
	void Material::Destroy()
	{
		if (!IsValid())
		{
			LOG_WARN("Attempting to destroy invalid material");
			return;
		}
		MaterialManager::DeleteMaterial(GetName());
	}


	
	// Getters:
	const std::string& Material::GetName() const
	{
		const std::string* pName = MaterialManager::GetMaterialName(m_materialId);
		if (pName == nullptr)
			throw std::runtime_error("Material::GetName() failed. Material is invalid or expired.");
		return *pName;
	}
	emberCommon::MaterialType Material::GetMaterialType() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Material::GetMaterialType() failed. Material is invalid or expired.");
		return pIMaterial->GetMaterialType();
	}
	emberCommon::CullMode Material::GetCullMode() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Material::GetCullMode() failed. Material is invalid or expired.");
		return pIMaterial->GetCullMode();
	}
	bool Material::IsValid() const
	{
		return GetInterfaceHandle() != nullptr;
	}



	// Debugging:
	void Material::Print() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Material::Print() failed. Material is invalid or expired.");
		pIMaterial->Print();
	}



	// Private methods:
	emberBackendInterface::IMaterial* Material::GetInterfaceHandle() const
	{
		return MaterialManager::GetMaterialInterface(m_materialId);
	}
}