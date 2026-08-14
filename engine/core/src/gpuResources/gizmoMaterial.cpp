#include "gizmoMaterial.h"
#include "iMaterial.h"
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
			throw std::runtime_error("GizmoMaterial::GetRenderMode() failed. Material is invalid or expired.");
		return pIMaterial->GetGizmoRenderMode();
	}
	const emberCommon::GizmoRenderState& GizmoMaterial::GetRenderState() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("GizmoMaterial::GetRenderState() failed. Material is invalid or expired.");
		return pIMaterial->GetGizmoRenderState();
	}
	int32_t GizmoMaterial::GetRenderQueue() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("GizmoMaterial::GetRenderQueue() failed. Material is invalid or expired.");
		return pIMaterial->GetRenderQueue();
	}
	bool GizmoMaterial::GetIsTransparent() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("GizmoMaterial::GetIsTransparent() failed. Material is invalid or expired.");
		return pIMaterial->IsTransparent();
	}



	// Setters:
	void GizmoMaterial::SetRenderMode(emberCommon::GizmoRenderMode renderMode)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("GizmoMaterial::SetRenderMode(...) failed. Material is invalid or expired.");
		pIMaterial->SetGizmoRenderMode(renderMode);
	}
	void GizmoMaterial::SetCullMode(emberCommon::CullMode cullMode)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("GizmoMaterial::SetCullMode(...) failed. Material is invalid or expired.");
		pIMaterial->SetCullMode(cullMode);
	}
	void GizmoMaterial::SetRenderQueue(int32_t renderQueue)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("GizmoMaterial::SetRenderQueue(...) failed. Material is invalid or expired.");
		pIMaterial->SetRenderQueue(renderQueue);
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