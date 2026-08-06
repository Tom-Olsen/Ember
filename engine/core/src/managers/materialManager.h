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



// Forward decleration:
namespace emberBackendInterface
{
    class IMaterial;
}



namespace emberCore
{
    /// <summary>
    /// Purely static class that takes care of lifetime of all Material objects.
    /// Material is a none-owning wrapper around IMaterial. The MaterialManager owns the IMaterial objects.
    /// </summary>
    class EMBER_CORE_API MaterialManager
    {
    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<std::string, std::unique_ptr<emberBackendInterface::IMaterial>> s_materialInterfacesMap;

    public: // Methods
        // Initialization/Cleanup:
        static void Init();
        static void Clear();

		// Creators:
        static ForwardMaterial CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
        static GizmoMaterial CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
        static ShadowMaterial CreateShadowMaterial(const std::string& name, const std::filesystem::path& vertexSpv);

		// Cloners:
		static ForwardMaterial CloneForwardMaterial(const std::string& name, const ForwardMaterial& sourceMaterial);
		static ForwardMaterial CloneForwardMaterial(const std::string& name, const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode);
		static GizmoMaterial CloneGizmoMaterial(const std::string& name, const GizmoMaterial& sourceMaterial);
		static GizmoMaterial CloneGizmoMaterial(const std::string& name, const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode);
		static ShadowMaterial CloneShadowMaterial(const std::string& name, const ShadowMaterial& sourceMaterial);
        
		// Getters: (throw on fail)
		static Material GetMaterial(const std::string& name);
        static ForwardMaterial GetForwardMaterial(const std::string& name);
        static GizmoMaterial GetGizmoMaterial(const std::string& name);
        static ShadowMaterial GetShadowMaterial(const std::string& name);
        
		// Try getters: (return invalid Material on fail)
        static Material TryGetMaterial(const std::string& name);
        static ForwardMaterial TryGetForwardMaterial(const std::string& name);
        static GizmoMaterial TryGetGizmoMaterial(const std::string& name);
        static ShadowMaterial TryGetShadowMaterial(const std::string& name);

		// Deleter:
		static void DeleteMaterial(const std::string& name);

        // Debugging:
        static void Print();

    private: // Methods
        // Delete all constructors:
        MaterialManager() = delete;
        MaterialManager(const MaterialManager&) = delete;
        MaterialManager& operator=(const MaterialManager&) = delete;
        MaterialManager(MaterialManager&&) = delete;
        MaterialManager& operator=(MaterialManager&&) = delete;
        ~MaterialManager() = delete;
    };
}