#include "gizmoMaterial.h"
#include "iMaterial.h"
#include <stdexcept>



namespace emberCore
{
	// Private methods:
	GizmoMaterial::GizmoMaterial(emberBackendInterface::IMaterial* pIMaterial)
		: Material(pIMaterial)
	{
		if (pIMaterial->GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("GizmoMaterial::GizmoMaterial(...) failed. IMaterial is not a gizmo material.");
	}



	// Public methods:
	// Constructor/Destructor:
	GizmoMaterial::GizmoMaterial()
		: Material()
	{

	}
	GizmoMaterial::~GizmoMaterial()
	{

	}



	// Getters:
	emberCommon::GizmoRenderMode GizmoMaterial::GetRenderMode() const
	{
		return m_pIMaterial->GetGizmoRenderMode();
	}
	const emberCommon::GizmoRenderState& GizmoMaterial::GetRenderState() const
	{
		return m_pIMaterial->GetGizmoRenderState();
	}
	int32_t GizmoMaterial::GetRenderQueue() const
	{
		return m_pIMaterial->GetRenderQueue();
	}
	bool GizmoMaterial::GetIsTransparent() const
	{
		return m_pIMaterial->IsTransparent();
	}



	// Setters:
	void GizmoMaterial::SetRenderMode(emberCommon::GizmoRenderMode renderMode)
	{
		m_pIMaterial->SetGizmoRenderMode(renderMode);
	}
	void GizmoMaterial::SetCullMode(emberCommon::CullMode cullMode)
	{
		m_pIMaterial->SetCullMode(cullMode);
	}
	void GizmoMaterial::SetRenderQueue(int32_t renderQueue)
	{
		m_pIMaterial->SetRenderQueue(renderQueue);
	}
}