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
		MaterialAsset Load(const std::filesystem::path& path);

	private: // Methods:
		void ValidateRootMembers(const nlohmann::json& json, const std::filesystem::path& path);
		std::string GetRequiredString(const nlohmann::json& json, const std::filesystem::path& path, std::string_view memberName);
		emberCommon::MaterialPass ParseMaterialPass(const std::filesystem::path& path, const std::string& value);
		void SetRenderModeSettings(MaterialAsset& materialAsset, const nlohmann::json& json, const std::filesystem::path& path, emberCommon::MaterialPass materialPass);
		
		emberCommon::ForwardRenderMode ParseForwardRenderMode(const std::filesystem::path& path, const std::string& value);
		emberCommon::GizmoRenderMode ParseGizmoRenderMode(const std::filesystem::path& path, const std::string& value);
		emberCommon::ShaderStage ParseShaderStage(const std::filesystem::path& path, const std::string& value);
		void SetShaderStages(MaterialAsset& materialAsset, const nlohmann::json& json, const std::filesystem::path& path);
		void ValidateShaderStages(const MaterialAsset& materialAsset, const std::filesystem::path& path);
	};
}