#pragma once
#include "iMaterialManager.h"
#include "commonMaterialPass.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



namespace vulkanRendererBackend
{
	class Material;
	class MaterialShaderManager;



	class VULKAN_RENDERER_API MaterialManager : public emberBackendInterface::IMaterialManager
	{
		friend class MaterialShaderManager;

	private: // Structs:
		struct ManagedMaterial
		{
			std::string name;
			bool isAccessible;
			bool isDeletable;
			bool isMutable;
			emberCommon::MaterialShaderId materialShaderId;
			emberCommon::MaterialId shadowMaterialId;
			std::unique_ptr<Material> pMaterial;
		};
		struct MaterialSlot
		{
			uint32_t generation;
			ManagedMaterial managedMaterial;
		};

	private: // Members:
		MaterialShaderManager* m_pMaterialShaderManager;
		emberCommon::MaterialId m_defaultShadowMaterialId;
		std::unordered_map<std::string, uint32_t> m_materialIdsMap;
		std::vector<MaterialSlot> m_materialSlots;
		std::vector<uint32_t> m_freeMaterialIds;

	public: // Methods:
		// Constructor/Destructor:
		MaterialManager(MaterialShaderManager* pMaterialShaderManager);
		~MaterialManager() override;

		// Creators:
		emberCommon::MaterialId CreateGizmoMaterial(emberCommon::MaterialShaderId materialShaderId, emberCommon::GizmoRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId CreateOutlineMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name);
		emberCommon::MaterialId CreateShadowMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name) override;
		emberCommon::MaterialId CreateDeferredGeometryMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name) override;
		emberCommon::MaterialId CreateDeferredLightingMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name);
		emberCommon::MaterialId CreateForwardMaterial(emberCommon::MaterialShaderId materialShaderId, emberCommon::ForwardRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId CreatePresentMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name);

		// Cloners:
		emberCommon::MaterialId CloneGizmoMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneGizmoMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::GizmoRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId CloneOutlineMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name);
		emberCommon::MaterialId CloneShadowMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneDeferredGeometryMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneDeferredGeometryMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneDeferredLightingMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name);
		emberCommon::MaterialId CloneForwardMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneForwardMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::ForwardRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId CloneForwardMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneForwardMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, emberCommon::ForwardRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId ClonePresentMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name);

		// Getters:
		emberCommon::MaterialId TryGetMaterialId(const std::string& name) const override;
		emberCommon::MaterialId GetDefaultShadowMaterialId() const override;
		emberCommon::MaterialId TryGetShadowMaterialId(emberCommon::MaterialId surfaceMaterialId) override;
		emberBackendInterface::IMaterial* TryGetMaterial(emberCommon::MaterialId materialId) const override;
		const std::string* TryGetMaterialName(emberCommon::MaterialId materialId) const override;
		const emberCommon::MaterialShaderId* TryGetMaterialShaderId(emberCommon::MaterialId materialId) const override;
		bool IsMaterialMutable(emberCommon::MaterialId materialId) const override;

		// Setters:
		void SetAccessRights(emberCommon::MaterialId materialId, bool isAccessible, bool isDeletable, bool isMutable);
		void SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId) override;
		void ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId) override;

		// Deleter:
		void DeleteMaterial(emberCommon::MaterialId materialId) override;

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Management:
		emberCommon::MaterialId AddMaterial(const std::string& name, bool isAccessible, bool isDeletable, bool isMutable, emberCommon::MaterialShaderId materialShaderId, std::unique_ptr<Material> pMaterial);
		void DestroyMaterial(std::unique_ptr<Material> pMaterial);
		void Clear();
		emberCommon::MaterialId FindMaterialId(const std::string& name) const;
		bool IsMaterialShaderInUse(emberCommon::MaterialShaderId materialShaderId) const;
		bool IsSurfaceMaterialPass(emberCommon::MaterialPass materialPass) const;
	};
}