#include "computeShaderAssetLoader.h"
#include "json.h"
#include "jsonUtility.h"
#include "shaderFeatureMacros.h"
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>



namespace emberAssetLoader
{
	// Public methods:
	ComputeShaderAsset ComputeShaderAssetLoader::Load(const std::filesystem::path& path)
	{
		nlohmann::json json = JsonUtility::LoadObject(path);
		ValidateRootMembers(json, path);
		ComputeShaderAsset computeShaderAsset;
		computeShaderAsset.computeShaderName = JsonUtility::GetRequiredString(json, path, "computeShaderName");
		computeShaderAsset.sourcePath = JsonUtility::ResolveFilePath(path, JsonUtility::GetRequiredString(json, path, "sourcePath"), "sourcePath");
		computeShaderAsset.binaryPath = JsonUtility::ResolveFilePath(path, JsonUtility::GetRequiredString(json, path, "binaryPath"), "binaryPath");
		computeShaderAsset.accessRights = JsonUtility::GetResourceAccessRights(json, path);
		SetFeatures(computeShaderAsset);
		return computeShaderAsset;
	}



	// Private methods:
	void ComputeShaderAssetLoader::ValidateRootMembers(const nlohmann::json& json, const std::filesystem::path& path)
	{
		for (auto iterator = json.begin(); iterator != json.end(); iterator++)
		{
			const std::string& memberName = iterator.key();
			if (memberName != "computeShaderName" && memberName != "sourcePath" && memberName != "binaryPath" && memberName != "accessRights")
				throw std::runtime_error("ComputeShaderAssetLoader::Load(...) failed for '" + path.string() + "'. Unknown member '" + memberName + "'.");
		}
	}
	void ComputeShaderAssetLoader::SetFeatures(ComputeShaderAsset& computeShaderAsset)
	{
		// TODO: Packaged builds should load feature metadata generated during shader compilation instead of requiring HLSL source files at runtime.
		std::ifstream sourceFile(computeShaderAsset.sourcePath);
		if (!sourceFile.is_open())
			throw std::runtime_error("ComputeShaderAssetLoader::SetFeatures(...) failed. Could not open '" + computeShaderAsset.sourcePath.string() + "'.");

		std::stringstream stream;
		stream << sourceFile.rdbuf();
		std::string_view macroName = emberCommon::ToString(emberCommon::ShaderFeatureMacro::sceneColorAccess);
		std::string macroValue = FindMacroValue(stream.str(), macroName, computeShaderAsset.sourcePath);
		computeShaderAsset.features.sceneColorAccess = ParseSceneColorAccess(macroValue, computeShaderAsset.sourcePath);
	}
	std::string ComputeShaderAssetLoader::FindMacroValue(const std::string& sourceCode, std::string_view macroName, const std::filesystem::path& sourcePath)
	{
		std::regex macroRegex("^\\s*#\\s*define\\s+" + std::string(macroName) + "\\s+([^\\s/]+)", std::regex_constants::ECMAScript);
		std::stringstream sourceStream(sourceCode);
		std::string line;
		std::string macroValue;
		while (std::getline(sourceStream, line))
		{
			std::smatch match;
			if (!std::regex_search(line, match, macroRegex))
				continue;
			if (!macroValue.empty())
				throw std::runtime_error("ComputeShaderAssetLoader::FindMacroValue(...) failed. Macro '" + std::string(macroName) + "' is defined multiple times in '" + sourcePath.string() + "'.");
			macroValue = match[1].str();
		}
		return macroValue;
	}
	emberCommon::SceneColorAccess ComputeShaderAssetLoader::ParseSceneColorAccess(const std::string& macroValue, const std::filesystem::path& sourcePath)
	{
		if (macroValue.empty() || macroValue == emberCommon::ToString(emberCommon::ShaderFeatureMacro::sceneColorAccessNone) || macroValue == std::to_string(EMBER_SCENE_COLOR_ACCESS_NONE))
			return emberCommon::SceneColorAccess::none;
		if (macroValue == emberCommon::ToString(emberCommon::ShaderFeatureMacro::sceneColorAccessRead) || macroValue == std::to_string(EMBER_SCENE_COLOR_ACCESS_READ))
			return emberCommon::SceneColorAccess::read;
		if (macroValue == emberCommon::ToString(emberCommon::ShaderFeatureMacro::sceneColorAccessInPlace) || macroValue == std::to_string(EMBER_SCENE_COLOR_ACCESS_IN_PLACE))
			return emberCommon::SceneColorAccess::inPlace;
		if (macroValue == emberCommon::ToString(emberCommon::ShaderFeatureMacro::sceneColorAccessOutOfPlace) || macroValue == std::to_string(EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE))
			return emberCommon::SceneColorAccess::outOfPlace;

		throw std::runtime_error("ComputeShaderAssetLoader::ParseSceneColorAccess(...) failed. Source file '" + sourcePath.string() + "' uses unsupported " + std::string(emberCommon::ToString(emberCommon::ShaderFeatureMacro::sceneColorAccess)) + " value '" + macroValue + "'.");
	}
}