#pragma once
#include "jsonFwd.h"
#include "materialAsset.h"
#include <filesystem>
#include <string>
#include <string_view>



namespace emberAssetLoader
{
	class MaterialAssetLoader
	{
	public: // Methods:
		static MaterialAsset Load(const std::filesystem::path& path);

	private: // Methods:
		static void ValidateRootMembers(const nlohmann::json& json, const std::filesystem::path& path);
		static std::string GetRequiredString(const nlohmann::json& json, const std::filesystem::path& path, std::string_view memberName);
		static emberCommon::MaterialPass ParseMaterialPass(const std::filesystem::path& path, const std::string& value);
		static void SetRenderModeSettings(MaterialAsset& materialAsset, const nlohmann::json& json, const std::filesystem::path& path, emberCommon::MaterialPass materialPass);
		static emberCommon::GizmoRenderMode ParseGizmoRenderMode(const std::filesystem::path& path, const std::string& value);
		static emberCommon::ForwardRenderMode ParseForwardRenderMode(const std::filesystem::path& path, const std::string& value);
		static void SetShaderStages(MaterialAsset& materialAsset, const nlohmann::json& json, const std::filesystem::path& path);
		static emberCommon::ShaderStage ParseShaderStage(const std::filesystem::path& path, const std::string& value);
		static void ValidateShaderStages(const MaterialAsset& materialAsset, const std::filesystem::path& path);
	};
}