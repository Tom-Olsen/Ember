#pragma once
#include "iMaterialManager.h"
#include "commonMaterialPass.h"
#include "commonResourceAccessRights.h"
#include "vulkanMaterialShaderId.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>



// Forward declarations:
namespace emberAssetLoader
{
	struct MaterialAsset;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class MaterialShaderManager;



	class VULKAN_RENDERER_API MaterialManager : public emberBackendInterface::IMaterialManager
	{
		// Friends:
		friend class MaterialShaderManager;

	private: // Structs:
		struct ManagedMaterial
		{
			std::string name;
			emberCommon::ResourceAccessRights accessRights;
			MaterialShaderId materialShaderId;
			emberCommon::MaterialId shadowMaterialId;
			std::unique_ptr<Material> pMaterial;

			// Constructor:
			ManagedMaterial(std::string name, const emberCommon::ResourceAccessRights& accessRights, MaterialShaderId materialShaderId, emberCommon::MaterialId shadowMaterialId, std::unique_ptr<Material> pMaterial);
			
			// Non-copyable:
			ManagedMaterial(const ManagedMaterial& other) = delete;
			ManagedMaterial& operator=(const ManagedMaterial& other) = delete;
			
			// Movable:
			ManagedMaterial(ManagedMaterial&& other) noexcept = default;
			ManagedMaterial& operator=(ManagedMaterial&& other) noexcept = default;
		};
		struct MaterialSlot
		{
			uint32_t generation;
			ManagedMaterial managedMaterial;

			// Constructor:
			MaterialSlot(uint32_t generation, ManagedMaterial managedMaterial);
			
			// Non-copyable:
			MaterialSlot(const MaterialSlot& other) = delete;
			MaterialSlot& operator=(const MaterialSlot& other) = delete;
			
			// Movable:
			MaterialSlot(MaterialSlot&& other) noexcept = default;
			MaterialSlot& operator=(MaterialSlot&& other) noexcept = default;
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
		
		// Non-copyable:
		MaterialManager(const MaterialManager& other) = delete;
		MaterialManager& operator=(const MaterialManager& other) = delete;
		
		// Non-movable:
		MaterialManager(MaterialManager&& other) = delete;
		MaterialManager& operator=(MaterialManager&& other) = delete;

		// Creation/Cloning:
		void InitializeDefaultMaterials() override;
		emberCommon::MaterialId CreateMaterial(const emberAssetLoader::MaterialAsset& materialAsset) override;
		emberCommon::MaterialId CloneGizmoMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::GizmoRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId CloneGizmoMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, emberCommon::GizmoRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId CloneOutlineMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name);
		emberCommon::MaterialId CloneShadowMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneDeferredGeometryMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneDeferredGeometryMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, const std::string& name) override;
		emberCommon::MaterialId CloneDeferredLightingMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name);
		emberCommon::MaterialId CloneForwardMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::ForwardRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId CloneForwardMaterialWithDefaultBindings(emberCommon::MaterialId sourceMaterialId, emberCommon::ForwardRenderMode renderMode, const std::string& name) override;
		emberCommon::MaterialId ClonePresentMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name);

		// Getters:
		emberCommon::MaterialId TryGetMaterialId(const std::string& name) const override;
		emberCommon::MaterialId GetDefaultShadowMaterialId() const override;
		emberCommon::MaterialId TryGetShadowMaterialId(emberCommon::MaterialId surfaceMaterialId) override;
		emberBackendInterface::IMaterial* TryGetMaterial(emberCommon::MaterialId materialId) const override;
		const std::string* TryGetMaterialName(emberCommon::MaterialId materialId) const override;
		bool IsMaterialMutable(emberCommon::MaterialId materialId) const override;

		// Setters:
		void SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId) override;
		void ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId) override;

		// Deleter:
		void DeleteMaterial(emberCommon::MaterialId materialId) override;

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Creators:
		emberCommon::MaterialId CreateGizmoMaterial(MaterialShaderId materialShaderId, emberCommon::GizmoRenderMode renderMode, const std::string& name);
		emberCommon::MaterialId CreateOutlineMaterial(MaterialShaderId materialShaderId, const std::string& name);
		emberCommon::MaterialId CreateShadowMaterial(MaterialShaderId materialShaderId, const std::string& name);
		emberCommon::MaterialId CreateDeferredGeometryMaterial(MaterialShaderId materialShaderId, const std::string& name);
		emberCommon::MaterialId CreateDeferredLightingMaterial(MaterialShaderId materialShaderId, const std::string& name);
		emberCommon::MaterialId CreateForwardMaterial(MaterialShaderId materialShaderId, emberCommon::ForwardRenderMode renderMode, const std::string& name);
		emberCommon::MaterialId CreatePresentMaterial(MaterialShaderId materialShaderId, const std::string& name);

		// Management:
		void Clear();
		emberCommon::MaterialId AddMaterial(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, MaterialShaderId materialShaderId, std::unique_ptr<Material> pMaterial);
		void DestroyMaterial(std::unique_ptr<Material> pMaterial);
		emberCommon::MaterialId FindMaterialId(const std::string& name) const;
		Material* GetMaterial(const std::string& name) const;
		bool IsMaterialShaderInUse(MaterialShaderId materialShaderId) const;
		bool IsSurfaceMaterialPass(emberCommon::MaterialPass materialPass) const;
		const MaterialShaderId* TryGetMaterialShaderId(emberCommon::MaterialId materialId) const;
		void SetAccessRights(emberCommon::MaterialId materialId, const emberCommon::ResourceAccessRights& accessRights);
	};
}