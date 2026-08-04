#include "material.h"
#include "forwardMaterial.h"
#include "gizmoMaterial.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"
#include "shadowMaterial.h"



namespace emberCore
{
	// Private methods:
	emberBackendInterface::IMaterial* const Material::GetInterfaceHandle() const
	{
		return m_pIMaterial;
	}
	Material::Material(emberBackendInterface::IMaterial* pIMaterial)
        : Shader(pIMaterial->GetShaderDescriptorSetBinding())
	{
		m_pIMaterial = pIMaterial;
	}



	// Public methods:
	// Constructor/Destructor:
	Material::Material()
	{
		m_pIMaterial = nullptr;
	}
	Material::~Material()
	{

	}



	// Creation/Destruction: (register/delete from MaterialManager)
	ForwardMaterial Material::CreateForward(emberCommon::ForwardRenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		return MaterialManager::CreateForwardMaterial(renderMode, name, vertexSpv, fragmentSpv);
	}
	GizmoMaterial Material::CreateGizmo(emberCommon::GizmoRenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		return MaterialManager::CreateGizmoMaterial(renderMode, name, vertexSpv, fragmentSpv);
	}
	ShadowMaterial Material::CreateShadow(const std::string& name, const std::filesystem::path& vertexSpv)
	{
		return MaterialManager::CreateShadowMaterial(name, vertexSpv);
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
		return m_pIMaterial->GetName();
	}
	emberCommon::MaterialType Material::GetMaterialType() const
	{
		return m_pIMaterial->GetMaterialType();
	}
	emberCommon::CullMode Material::GetCullMode() const
	{
		return m_pIMaterial->GetCullMode();
	}
	bool Material::IsValid() const
	{
		return m_pIMaterial != nullptr;
	}



	// Debugging:
	void Material::Print() const
	{
		m_pIMaterial->Print();
	}
}