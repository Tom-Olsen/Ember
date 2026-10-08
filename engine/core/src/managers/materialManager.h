#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialCloneInfo.h"
#include "commonMaterialId.h"
#include "commonMaterialPass.h"
#include "commonMaterialShaderId.h"
#include "commonResourceAccessRights.h"
#include "deferredMaterial.h"
#include "emberCoreExport.h"
#include "forwardMaterial.h"
#include "gizmoMaterial.h"
#include "material.h"
#include "namedSlotMap.h"
#include "shadowMaterial.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>



// Forward declarations:
namespace emberAssetLoader
{
	struct MaterialAsset;
}
namespace emberBackendInterface
{
	class IMaterial;
}



namespace emberCore
{
	// Forward declarations:
	class Core;



	class EMBER_CORE_API MaterialManager
	{
		// Friends:
		friend class Core;
		friend class DeferredMaterial;
		friend class ForwardMaterial;
		friend class Material;
		friend class Renderer;

	private: // Structs:
		struct ManagedMaterial
		{
			emberCommon::ResourceAccessRights accessRights;
			emberCommon::MaterialShaderId materialShaderId;
			emberCommon::MaterialId shadowMaterialId;
			std::unique_ptr<emberBackendInterface::IMaterial> pOwnedIMaterial;
			emberBackendInterface::IMaterial* pIMaterial;

			// Constructor/Destructor:
			ManagedMaterial();
			ManagedMaterial(const emberCommon::ResourceAccessRights& accessRights, emberCommon::MaterialShaderId materialShaderId, emberCommon::MaterialId shadowMaterialId, std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial);
			~ManagedMaterial();

			// Non-copyable:
			ManagedMaterial(const ManagedMaterial&) = delete;
			ManagedMaterial& operator=(const ManagedMaterial&) = delete;

			// Movable:
			ManagedMaterial(ManagedMaterial&&) noexcept;
			ManagedMaterial& operator=(ManagedMaterial&&) noexcept;
		};

	private: // Members:
		static bool s_isInitialized;
		static emberBackendInterface::IMaterial* s_pIErrorMaterial;
		static emberBackendInterface::IMaterial* s_pIErrorGizmoMaterial;
		static emberCommon::MaterialId s_defaultShadowMaterialId;
		static emberDataStructures::NamedSlotMap<emberCommon::MaterialId, ManagedMaterial> s_materialSlotMap;

	public: // Methods:
		// Asset loading:
		static void LoadMaterialAssets(const std::filesystem::path& directoryPath);

		// Cloners:
		static GizmoMaterial CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, const std::string& name);
		static GizmoMaterial CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name);
		static GizmoMaterial CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, const std::string& name);
		static GizmoMaterial CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name);
		static ShadowMaterial CloneShadowMaterial(const ShadowMaterial& sourceMaterial, const std::string& name);
		static DeferredMaterial CloneDeferredGeometryMaterial(const DeferredMaterial& sourceMaterial, const std::string& name);
		static DeferredMaterial CloneDeferredGeometryMaterialWithDefaultBindings(const DeferredMaterial& sourceMaterial, const std::string& name);
		static ForwardMaterial CloneForwardMaterial(const ForwardMaterial& sourceMaterial, const std::string& name);
		static ForwardMaterial CloneForwardMaterial(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name);
		static ForwardMaterial CloneForwardMaterialWithDefaultBindings(const ForwardMaterial& sourceMaterial, const std::string& name);
		static ForwardMaterial CloneForwardMaterialWithDefaultBindings(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name);

		// Getters:
		static Material TryGetMaterial(const std::string& name);
		static GizmoMaterial TryGetGizmoMaterial(const std::string& name);
		static ShadowMaterial TryGetShadowMaterial(const std::string& name);
		static ShadowMaterial GetDefaultShadowMaterial();
		static DeferredMaterial TryGetDeferredMaterial(const std::string& name);
		static ForwardMaterial TryGetForwardMaterial(const std::string& name);

		// Deleter:
		static void DeleteMaterial(const std::string& name);

		// Debugging:
		static void Print();

	private: // Methods:
		// Initialization/Cleanup:
		static void Init();
		static void Clear();

		// Creation/Cloning:
		static emberCommon::MaterialId CreateMaterial(const emberAssetLoader::MaterialAsset& materialAsset);
		static emberCommon::MaterialId CloneMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::MaterialPass expectedMaterialPass, emberCommon::MaterialBindingCloneMode bindingCloneMode, const std::string& name);

		// Getters:
		static Material GetMaterial(emberCommon::MaterialId materialId);
		static GizmoMaterial GetGizmoMaterial(emberCommon::MaterialId materialId);
		static ShadowMaterial GetShadowMaterial(emberCommon::MaterialId materialId);
		static ShadowMaterial GetShadowMaterialForSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId);
		static DeferredMaterial GetDeferredMaterial(emberCommon::MaterialId materialId);
		static ForwardMaterial GetForwardMaterial(emberCommon::MaterialId materialId);
		static emberCommon::MaterialId TryGetMaterialId(const std::string& name);
		static emberBackendInterface::IMaterial* TryGetMaterialInterface(emberCommon::MaterialId materialId);
		static std::optional<std::string> TryGetMaterialName(emberCommon::MaterialId materialId);
		static emberCommon::MaterialId TryGetShadowMaterialIdOfSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId);
		static emberCommon::MaterialShaderId TryGetMaterialShaderId(emberCommon::MaterialId materialId);
		static bool IsMaterialMutable(emberCommon::MaterialId materialId);

		// Setters:
		static void SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId);
		static void ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId);

		// Deleter:
		static void DeleteMaterial(emberCommon::MaterialId materialId);

		// Management:
		static emberCommon::MaterialId AddMaterial(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, emberCommon::MaterialShaderId materialShaderId, std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial);
		static std::unique_ptr<emberBackendInterface::IMaterial> TakeMaterialOwnership(const std::string& name);
		static void RetireMaterial(std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial);
		static emberCommon::MaterialId FindMaterialId(const std::string& name);

		// Delete all constructors:
		MaterialManager() = delete;
		MaterialManager(const MaterialManager&) = delete;
		MaterialManager& operator=(const MaterialManager&) = delete;
		MaterialManager(MaterialManager&&) = delete;
		MaterialManager& operator=(MaterialManager&&) = delete;
		~MaterialManager() = delete;
	};
}