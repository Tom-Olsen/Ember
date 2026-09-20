#include "materialAsset.h"
#include <stdexcept>



namespace emberAssetLoader
{
	emberCommon::MaterialPass MaterialAsset::GetMaterialPass() const
	{
		if (std::holds_alternative<GizmoSettings>(renderModeSettings))
			return emberCommon::MaterialPass::gizmo;
		if (std::holds_alternative<OutlineSettings>(renderModeSettings))
			return emberCommon::MaterialPass::outline;
		if (std::holds_alternative<ShadowSettings>(renderModeSettings))
			return emberCommon::MaterialPass::shadow;
		if (std::holds_alternative<DeferredGeometrySettings>(renderModeSettings))
			return emberCommon::MaterialPass::deferredGeometry;
		if (std::holds_alternative<DeferredLightingSettings>(renderModeSettings))
			return emberCommon::MaterialPass::deferredLighting;
		if (std::holds_alternative<ForwardSettings>(renderModeSettings))
			return emberCommon::MaterialPass::forward;
		if (std::holds_alternative<PresentSettings>(renderModeSettings))
			return emberCommon::MaterialPass::present;

		throw std::runtime_error("MaterialAsset::GetMaterialPass() failed. Material settings are invalid.");
	}
}