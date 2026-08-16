#include "gizmoMaterial.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	GizmoMaterial::GizmoMaterial()
		: Material()
	{

	}
	GizmoMaterial::~GizmoMaterial()
	{

	}



	// Creation/Cloning:
	GizmoMaterial GizmoMaterial::CreateFromShader(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name)
	{
		return MaterialManager::CreateGizmoMaterial(renderMode, materialShader, name);
	}
	GizmoMaterial GizmoMaterial::Clone(const std::string& name) const
	{
		return MaterialManager::CloneGizmoMaterial(*this, name);
	}
	GizmoMaterial GizmoMaterial::Clone(const std::string& name, emberCommon::GizmoRenderMode renderMode) const
	{
		return MaterialManager::CloneGizmoMaterial(*this, renderMode, name);
	}



	// Getters:
	emberCommon::GizmoRenderMode GizmoMaterial::GetRenderMode() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("GizmoMaterial::GetRenderMode() failed. Material is invalid or expired.");
			return emberCommon::GizmoRenderMode::count;
		}
		return pIMaterial->GetGizmoRenderMode();
	}
	const emberCommon::GizmoRenderState* GizmoMaterial::GetRenderState() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("GizmoMaterial::GetRenderState() failed. Material is invalid or expired.");
			return nullptr;
		}
		return &pIMaterial->GetGizmoRenderState();
	}
	int32_t GizmoMaterial::GetRenderQueue() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("GizmoMaterial::GetRenderQueue() failed. Material is invalid or expired.");
			return 0;
		}
		return pIMaterial->GetRenderQueue();
	}
	bool GizmoMaterial::GetIsTransparent() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("GizmoMaterial::GetIsTransparent() failed. Material is invalid or expired.");
			return false;
		}
		return pIMaterial->IsTransparent();
	}



	// Setters:
	void GizmoMaterial::SetRenderMode(emberCommon::GizmoRenderMode renderMode)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			pIMaterial->SetGizmoRenderMode(renderMode);
		else
			LOG_WARN("GizmoMaterial::SetRenderMode(...) failed. Material is invalid, expired, or immutable.");
	}
	void GizmoMaterial::SetCullMode(emberCommon::CullMode cullMode)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			pIMaterial->SetCullMode(cullMode);
		else
			LOG_WARN("GizmoMaterial::SetCullMode(...) failed. Material is invalid, expired, or immutable.");
	}
	void GizmoMaterial::SetRenderQueue(int32_t renderQueue)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			pIMaterial->SetRenderQueue(renderQueue);
		else
			LOG_WARN("GizmoMaterial::SetRenderQueue(...) failed. Material is invalid, expired, or immutable.");
	}



	// Private methods:
	GizmoMaterial::GizmoMaterial(MaterialId materialId)
		: Material(materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("GizmoMaterial::GizmoMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("GizmoMaterial::GizmoMaterial(...) failed. IMaterial is not a gizmo material.");
	}
}