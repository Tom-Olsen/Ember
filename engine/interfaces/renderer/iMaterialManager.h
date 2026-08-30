#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialId.h"
#include "commonMaterialShaderId.h"
#include <string>



namespace emberBackendInterface
{
	class IMaterial;



	class IMaterialManager
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IMaterialManager() = default;

		// Creators:
		virtual emberCommon::MaterialId CreateGizmoMaterial(emberCommon::MaterialShaderId materialShaderId, emberCommon::GizmoRenderMode renderMode, const std::string& name) = 0;
		virtual emberCommon::MaterialId CreateShadowMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name) = 0;
		virtual emberCommon::MaterialId CreateDeferredGeometryMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name) = 0;
		virtual emberCommon::MaterialId CreateForwardMaterial(emberCommon::MaterialShaderId materialShaderId, emberCommon::ForwardRenderMode renderMode, const std::string& name) = 0;

		// Cloners:
		virtual emberCommon::MaterialId CloneGizmoMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) = 0;
		virtual emberCommon::MaterialId CloneGizmoMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::GizmoRenderMode renderMode, const std::string& name) = 0;
		virtual emberCommon::MaterialId CloneShadowMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) = 0;
		virtual emberCommon::MaterialId CloneDeferredGeometryMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) = 0;
		virtual emberCommon::MaterialId CloneDeferredGeometryMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, const std::string& name) = 0;
		virtual emberCommon::MaterialId CloneForwardMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) = 0;
		virtual emberCommon::MaterialId CloneForwardMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::ForwardRenderMode renderMode, const std::string& name) = 0;
		virtual emberCommon::MaterialId CloneForwardMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, const std::string& name) = 0;
		virtual emberCommon::MaterialId CloneForwardMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, emberCommon::ForwardRenderMode renderMode, const std::string& name) = 0;

		// Getters:
		virtual emberCommon::MaterialId TryGetMaterialId(const std::string& name) const = 0;
		virtual emberCommon::MaterialId GetDefaultShadowMaterialId() const = 0;
		virtual emberCommon::MaterialId TryGetShadowMaterialId(emberCommon::MaterialId surfaceMaterialId) = 0;
		virtual IMaterial* TryGetMaterial(emberCommon::MaterialId materialId) const = 0;
		virtual const std::string* TryGetMaterialName(emberCommon::MaterialId materialId) const = 0;
		virtual const emberCommon::MaterialShaderId* TryGetMaterialShaderId(emberCommon::MaterialId materialId) const = 0;
		virtual bool IsMaterialMutable(emberCommon::MaterialId materialId) const = 0;

		// Setters:
		virtual void SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId) = 0;
		virtual void ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId) = 0;

		// Deleter:
		virtual void DeleteMaterial(emberCommon::MaterialId materialId) = 0;

		// Debugging:
		virtual void Print() const = 0;
	};
}