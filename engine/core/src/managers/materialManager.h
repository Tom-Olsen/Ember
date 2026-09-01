#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialId.h"
#include "commonMaterialShaderId.h"
#include "deferredMaterial.h"
#include "emberCoreExport.h"
#include "forwardMaterial.h"
#include "gizmoMaterial.h"
#include "material.h"
#include "shadowMaterial.h"
#include <filesystem>
#include <memory>
#include <string>



	// Forward declarations:
namespace emberBackendInterface
{
	class IMaterial;
	class IMaterialManager;
}
namespace emberAssetLoader
{
	class MaterialAsset;
}



namespace emberCore
{
	// Forward declarations:
	class MaterialShader;



	/// <summary>
	/// Purely static facade for the backend material manager.
	/// Material is a non-owning, generational handle to a backend-owned slot.
	/// </summary>
	class EMBER_CORE_API MaterialManager
	{
		// Friends:
		friend class DeferredMaterial;
		friend class ForwardMaterial;
		friend class Material;
		friend class Renderer;

	private: // Members:
		static std::unique_ptr<emberBackendInterface::IMaterialManager> s_pIMaterialManager;

	public: // Methods:
		// Creators:
		static Material CreateMaterial(const emberAssetLoader::MaterialAsset& materialAsset);
		static GizmoMaterial CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static GizmoMaterial CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
		static ShadowMaterial CreateShadowMaterial(const std::filesystem::path& vertexSpv, const std::string& name);
		static ShadowMaterial CreateShadowMaterial(const MaterialShader& materialShader, const std::string& name);
		static DeferredMaterial CreateDeferredGeometryMaterial(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static DeferredMaterial CreateDeferredGeometryMaterial(const MaterialShader& materialShader, const std::string& name);
		static ForwardMaterial CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static ForwardMaterial CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);

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

		// Getters:
		static Material GetMaterial(emberCommon::MaterialId materialId);
		static GizmoMaterial GetGizmoMaterial(emberCommon::MaterialId materialId);
		static ShadowMaterial GetShadowMaterial(emberCommon::MaterialId materialId);
		static ShadowMaterial GetShadowMaterialForSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId);
		static DeferredMaterial GetDeferredMaterial(emberCommon::MaterialId materialId);
		static ForwardMaterial GetForwardMaterial(emberCommon::MaterialId materialId);
		static emberCommon::MaterialId TryGetMaterialId(const std::string& name);
		static emberBackendInterface::IMaterial* TryGetMaterialInterface(emberCommon::MaterialId materialId);
		static const std::string* TryGetMaterialName(emberCommon::MaterialId materialId);
		static const emberCommon::MaterialShaderId* TryGetMaterialShaderId(emberCommon::MaterialId materialId);
		static emberCommon::MaterialId TryGetShadowMaterialIdOfSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId);
		static bool IsMaterialMutable(emberCommon::MaterialId materialId);

		// Setters:
		static void SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId);
		static void ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId);

		// Deleter:
		static void DeleteMaterial(emberCommon::MaterialId materialId);

		// Delete all constructors:
		MaterialManager() = delete;
		MaterialManager(const MaterialManager&) = delete;
		MaterialManager& operator=(const MaterialManager&) = delete;
		MaterialManager(MaterialManager&&) = delete;
		MaterialManager& operator=(MaterialManager&&) = delete;
		~MaterialManager() = delete;
	};
}