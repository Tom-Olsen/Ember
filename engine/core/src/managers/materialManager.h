#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "emberCoreExport.h"
#include "forwardMaterial.h"
#include "gizmoMaterial.h"
#include "material.h"
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
    /// Purely static class that takes care of lifetime of all Material objects.
    /// Material is a none-owning wrapper around IMaterial. The MaterialManager owns the IMaterial objects.
    /// </summary>
    class EMBER_CORE_API MaterialManager
	{
		// Friends:
		friend class ForwardMaterial;
		friend class Material;
		friend class MaterialShaderManager;
		
    private: // Structs:
		struct MaterialSlot
		{
			std::string name;
			std::string materialShaderName;
			uint32_t generation;
			std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial;
		};

    private: // Members:
	static bool s_isInitialized;
        static std::unordered_map<std::string, uint32_t> s_materialIdsMap;
		static std::vector<MaterialSlot> s_materialSlots;
		static std::vector<uint32_t> s_freeMaterialIds;

    public: // Methods:
        // Initialization/Cleanup:
        static void Init();
        static void Clear();

		// Creators:
		static Material CreateOutlineMaterial(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static Material CreateOutlineMaterial(const MaterialShader& materialShader, const std::string& name);
        static ForwardMaterial CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
        static ForwardMaterial CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
        static GizmoMaterial CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
        static GizmoMaterial CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
        static ShadowMaterial CreateShadowMaterial(const std::filesystem::path& vertexSpv, const std::string& name);
        static ShadowMaterial CreateShadowMaterial(const MaterialShader& materialShader, const std::string& name);
		static Material CreatePresentMaterial(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static Material CreatePresentMaterial(const MaterialShader& materialShader, const std::string& name);

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

		// Deleter:
		static void DeleteMaterial(const std::string& name);

        // Debugging:
        static void Print();

	private: // Methods:
		static Material::MaterialId GetMaterialId(const std::string& name);
		static Material::MaterialId GetMaterialId(emberBackendInterface::IMaterial* pIMaterial);
		static emberBackendInterface::IMaterial* GetMaterialInterface(Material::MaterialId materialId);
		static const std::string* GetMaterialName(Material::MaterialId materialId);
		static const std::string* GetMaterialShaderName(Material::MaterialId materialId);
		static bool MaterialShaderInUse(const std::string& materialShaderName);
		static void AddMaterial(const std::string& name, const std::string& materialShaderName, emberBackendInterface::IMaterial* pIMaterial, Material::MaterialId& materialId);

        // Delete all constructors:
        MaterialManager() = delete;
        MaterialManager(const MaterialManager&) = delete;
        MaterialManager& operator=(const MaterialManager&) = delete;
        MaterialManager(MaterialManager&&) = delete;
        MaterialManager& operator=(MaterialManager&&) = delete;
        ~MaterialManager() = delete;
    };
}