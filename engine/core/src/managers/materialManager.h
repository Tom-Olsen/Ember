#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "emberCoreExport.h"
#include "forwardMaterial.h"
#include "gizmoMaterial.h"
#include "material.h"
#include "materialId.h"
#include "materialRole.h"
#include "materialShaderId.h"
#include "shadowMaterial.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



// Forward decleration:
namespace emberBackendInterface
{
    class IMaterial;
}



namespace emberCore
{
	// Forward declarations:
	class MaterialShader;



	/// <summary>
	/// Purely static class that owns the lifetime of all backend materials.
	/// Material is a non-owning, generational handle to a slot owned by this manager.
	/// </summary>
    class EMBER_CORE_API MaterialManager
	{
		// Friends:
		friend class ForwardMaterial;
		friend class Material;
		friend class MaterialShaderManager;
		
    private: // Structs:
		struct ManagedMaterial
		{
			std::string name;
			MaterialRole roles;
			MaterialShaderId materialShaderId;
			std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial;
		};
		struct MaterialSlot
		{
			uint32_t generation;
			ManagedMaterial managedMaterial;
		};

    private: // Members:
		static bool s_isInitialized;
		static MaterialId s_defaultShadowMaterialId;
        static std::unordered_map<std::string, uint32_t> s_materialIdsMap;
		static std::vector<MaterialSlot> s_materialSlots;
		static std::vector<uint32_t> s_freeMaterialIds;

    public: // Methods:
        // Initialization/Cleanup:
        static void Init();
        static void Clear();

		// Creators:
        static ForwardMaterial CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
        static ForwardMaterial CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
        static GizmoMaterial CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
        static GizmoMaterial CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
        static ShadowMaterial CreateShadowMaterial(const std::filesystem::path& vertexSpv, const std::string& name);
        static ShadowMaterial CreateShadowMaterial(const MaterialShader& materialShader, const std::string& name);
		// Cloners:
		static ForwardMaterial CloneForwardMaterial(const ForwardMaterial& sourceMaterial, const std::string& name);
		static ForwardMaterial CloneForwardMaterial(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name);
		static GizmoMaterial CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, const std::string& name);
		static GizmoMaterial CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name);
		static ShadowMaterial CloneShadowMaterial(const ShadowMaterial& sourceMaterial, const std::string& name);
        
		// Getters:
		static Material GetMaterial(const std::string& name);
        static ForwardMaterial GetForwardMaterial(const std::string& name);
        static GizmoMaterial GetGizmoMaterial(const std::string& name);
        static ShadowMaterial GetShadowMaterial(const std::string& name);
		static ShadowMaterial GetDefaultShadowMaterial();

		// Deleter:
		static void DeleteMaterial(const std::string& name);

        // Debugging:
        static void Print();

	private: // Methods:
		// Creators:
		static Material CreateOutlineMaterial(const MaterialShader& materialShader, const std::string& name, MaterialRole roles);
		static ShadowMaterial CreateShadowMaterial(const MaterialShader& materialShader, const std::string& name, MaterialRole roles);
		static Material CreatePresentMaterial(const MaterialShader& materialShader, const std::string& name, MaterialRole roles);
		
		// Getters:
		static MaterialId GetMaterialId(const std::string& name);
		static MaterialId GetMaterialId(emberBackendInterface::IMaterial* pIMaterial);
		static emberBackendInterface::IMaterial* GetMaterialInterface(MaterialId materialId);
		static const std::string* GetMaterialName(MaterialId materialId);
		static const MaterialShaderId* GetMaterialShaderId(MaterialId materialId);
		
		// Bool checks:
		static bool HasMaterialRole(MaterialId materialId, MaterialRole role);
		static bool MaterialShaderInUse(MaterialShaderId materialShaderId);
		
		// Add/Delete material:
		static void AddMaterial(const std::string& name, MaterialRole roles, MaterialShaderId materialShaderId, emberBackendInterface::IMaterial* pIMaterial, MaterialId& materialId);
		static void DeleteMaterial(MaterialId materialId);

        // Delete all constructors:
        MaterialManager() = delete;
        MaterialManager(const MaterialManager&) = delete;
        MaterialManager& operator=(const MaterialManager&) = delete;
        MaterialManager(MaterialManager&&) = delete;
        MaterialManager& operator=(MaterialManager&&) = delete;
        ~MaterialManager() = delete;
    };
}