#pragma once
#include "emberCoreExport.h"
#include "materialShader.h"
#include "materialShaderId.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



// Forward decleration:
namespace emberBackendInterface
{
	class IMaterialShader;
}



namespace emberCore
{
	/// <summary>
	/// Purely static class that owns the lifetime of all backend material shaders.
	/// MaterialShader is a non-owning, generational handle to a slot owned by this manager.
	/// </summary>
	class EMBER_CORE_API MaterialShaderManager
	{
		// Friends:
		friend class MaterialManager;
		friend class MaterialShader;

	private: // Structs:
		struct ManagedMaterialShader
		{
			std::string name;
			std::unique_ptr<emberBackendInterface::IMaterialShader> pIMaterialShader;
		};
		struct MaterialShaderSlot
		{
			uint32_t generation;
			ManagedMaterialShader materialShader;
		};

	private: // Members:
		static bool s_isInitialized;
		static std::unordered_map<std::string, uint32_t> s_materialShaderIdsMap;
		static std::vector<MaterialShaderSlot> s_materialShaderSlots;
		static std::vector<uint32_t> s_freeMaterialShaderIds;

	public: // Methods:
		// Initialization/Cleanup:
		static void Init();
		static void Clear();

		// Creators:
		static MaterialShader CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name);
		static MaterialShader CreateDeferredGeometryMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);

		// Getters:
		static MaterialShader GetMaterialShader(const std::string& name);

		// Deleter:
		static void DeleteMaterialShader(const std::string& name);

		// Debugging:
		static void Print();

	private: // Methods:
		// Creators:
		static MaterialShader CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreateDeferredLightingMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		
		// Getters:
		static MaterialShaderId GetMaterialShaderId(const std::string& name);
		static emberBackendInterface::IMaterialShader* GetMaterialShaderInterface(MaterialShaderId materialShaderId);
		static const std::string* GetMaterialShaderName(MaterialShaderId materialShaderId);
		
		// Add/Delete material shader:
		static MaterialShader AddMaterialShader(const std::string& name, emberBackendInterface::IMaterialShader* pIMaterialShader);
		static void DeleteMaterialShader(MaterialShaderId materialShaderId);

		// Delete all constructors:
		MaterialShaderManager() = delete;
		MaterialShaderManager(const MaterialShaderManager&) = delete;
		MaterialShaderManager& operator=(const MaterialShaderManager&) = delete;
		MaterialShaderManager(MaterialShaderManager&&) = delete;
		MaterialShaderManager& operator=(MaterialShaderManager&&) = delete;
		~MaterialShaderManager() = delete;
	};
}