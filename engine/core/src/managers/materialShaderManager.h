#pragma once
#include "emberCoreExport.h"
#include "materialShader.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>



namespace emberCore
{
	class EMBER_CORE_API MaterialShaderManager
	{
	private: // Members:
		static bool s_isInitialized;
		static std::unordered_map<std::string, std::unique_ptr<MaterialShader>> s_materialShaders;

	public: // Methods:
		// Initialization/Cleanup:
		static void Init();
		static void Clear();

		// Creators:
		static MaterialShader& CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader& CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader& CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader& CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name);
		static MaterialShader& CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);

		// Add/Get/Delete:
		static void AddMaterialShader(MaterialShader&& materialShader);
		static MaterialShader& GetMaterialShader(const std::string& name);
		static MaterialShader* TryGetMaterialShader(const std::string& name);
		static void DeleteMaterialShader(const std::string& name);

		// Debugging:
		static void Print();

	private: // Methods:
		// Delete all constructors:
		MaterialShaderManager() = delete;
		MaterialShaderManager(const MaterialShaderManager&) = delete;
		MaterialShaderManager& operator=(const MaterialShaderManager&) = delete;
		MaterialShaderManager(MaterialShaderManager&&) = delete;
		MaterialShaderManager& operator=(MaterialShaderManager&&) = delete;
		~MaterialShaderManager() = delete;
	};
}