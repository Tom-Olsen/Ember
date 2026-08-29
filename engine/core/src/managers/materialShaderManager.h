#pragma once
#include "commonMaterialShaderId.h"
#include "emberCoreExport.h"
#include "materialShader.h"
#include <filesystem>
#include <memory>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IMaterialShader;
	class IMaterialShaderManager;
}



namespace emberCore
{
	/// <summary>
	/// Purely static facade for the backend material shader manager.
	/// MaterialShader is a non-owning, generational handle to a backend-owned slot.
	/// </summary>
	class EMBER_CORE_API MaterialShaderManager
	{
		friend class MaterialManager;
		friend class MaterialShader;
		friend class Renderer;

	private: // Members:
		static std::unique_ptr<emberBackendInterface::IMaterialShaderManager> s_pIMaterialShaderManager;

	public: // Methods:
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
		// Initialization/Cleanup:
		static void Init();
		static void Clear();

		// Getters:
		static emberCommon::MaterialShaderId GetMaterialShaderId(const std::string& name);
		static emberBackendInterface::IMaterialShaderManager* GetInterfaceHandle();
		static emberBackendInterface::IMaterialShader* GetMaterialShaderInterface(emberCommon::MaterialShaderId materialShaderId);
		static const std::string* GetMaterialShaderName(emberCommon::MaterialShaderId materialShaderId);

		// Deleter:
		static void DeleteMaterialShader(emberCommon::MaterialShaderId materialShaderId);

		// Delete all constructors:
		MaterialShaderManager() = delete;
		MaterialShaderManager(const MaterialShaderManager&) = delete;
		MaterialShaderManager& operator=(const MaterialShaderManager&) = delete;
		MaterialShaderManager(MaterialShaderManager&&) = delete;
		MaterialShaderManager& operator=(MaterialShaderManager&&) = delete;
		~MaterialShaderManager() = delete;
	};
}