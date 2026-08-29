#include "material.h"
#include "deferredMaterial.h"
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
	Material::Material(emberCommon::MaterialId materialId)
		: Shader()
	{
		m_materialId = materialId;
	}
	emberBackendInterface::IMaterial* Material::GetMutableInterfaceHandle() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr || !MaterialManager::IsMaterialMutable(m_materialId))
			return nullptr;
		return pIMaterial;
	}
	emberBackendInterface::IDescriptorSetBinding* Material::TryGetShaderDescriptorSetBinding() const
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			return pIMaterial->GetShaderDescriptorSetBinding();
		return nullptr;
	}



	// Public methods:
	// Constructor/Destructor:
	Material::Material()
	{
		m_materialId = emberCommon::invalidMaterialId;
	}
	Material::~Material()
	{

	}



	// Creation/Destruction:
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
	DeferredMaterial Material::CreateDeferredGeometry(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return MaterialManager::CreateDeferredGeometryMaterial(vertexSpv, fragmentSpv, debugName);
	}
	DeferredMaterial Material::CreateDeferredGeometry(const MaterialShader& materialShader, const std::string& debugName)
	{
		return MaterialManager::CreateDeferredGeometryMaterial(materialShader, debugName);
	}
	ForwardMaterial Material::CreateForward(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		return MaterialManager::CreateForwardMaterial(renderMode, vertexSpv, fragmentSpv, debugName);
	}
	ForwardMaterial Material::CreateForward(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& debugName)
	{
		return MaterialManager::CreateForwardMaterial(renderMode, materialShader, debugName);
	}
	void Material::Destroy()
	{
		if (!IsValid())
		{
			LOG_WARN("Attempting to destroy invalid material");
			return;
		}
		MaterialManager::DeleteMaterial(m_materialId);
	}



	// Getters:
	const std::string& Material::GetName() const
	{
		const std::string* pName = MaterialManager::TryGetMaterialName(m_materialId);
		if (pName == nullptr)
		{
			LOG_WARN("Material::GetName() failed. Material is invalid or expired.");
			static const std::string invalidName = "invalidOrExpired";
			return invalidName;
		}
		return *pName;
	}
	emberCommon::MaterialPass Material::GetMaterialPass() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("Material::GetMaterialPass() failed. Material is invalid or expired.");
			return emberCommon::MaterialPass::count;
		}
		return pIMaterial->GetMaterialPass();
	}
	emberCommon::CullMode Material::GetCullMode() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("Material::GetCullMode() failed. Material is invalid or expired.");
			return emberCommon::CullMode::count;
		}
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
		return MaterialManager::TryGetMaterialInterface(m_materialId);
	}
}