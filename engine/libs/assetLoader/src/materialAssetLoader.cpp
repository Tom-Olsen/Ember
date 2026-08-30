#include "materialAssetLoader.h"
#include "json.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>



namespace emberAssetLoader
{
	// Public methods:
	MaterialAsset MaterialAssetLoader::Load(const std::filesystem::path& path)
	{
		// Error handling:
		if (!std::filesystem::exists(path))
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. File does not exist.");
		if (!std::filesystem::is_regular_file(path))
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Path is not a regular file.");

		// File loading:
		std::ifstream file(path);
		if (!file.is_open())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Could not open file.");

		// Json loading:
		nlohmann::json json;
		try
		{
			file >> json;
		}
		catch (const nlohmann::json::exception& exception)
		{
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Invalid JSON: " + std::string(exception.what()));
		}
		if (!json.is_object())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Root value must be an object.");
			
		// Parse json file:
		ValidateRootMembers(json, path);
		MaterialAsset materialAsset = {};
		materialAsset.materialName = GetRequiredString(json, path, "materialName");
		materialAsset.materialShaderName = GetRequiredString(json, path, "materialShaderName");
		emberCommon::MaterialPass materialPass = ParseMaterialPass(path, GetRequiredString(json, path, "materialPass"));
		SetRenderModeSettings(materialAsset, json, path, materialPass);
		SetShaderStages(materialAsset, json, path);
		ValidateShaderStages(materialAsset, path);
		return materialAsset;
	}



	// Private methods:
	void MaterialAssetLoader::ValidateRootMembers(const nlohmann::json& json, const std::filesystem::path& path)
	{
		for (auto iterator = json.begin(); iterator != json.end(); iterator++)
		{
			const std::string& memberName = iterator.key();
			if (memberName != "materialName"
				&& memberName != "materialShaderName"
				&& memberName != "materialPass"
				&& memberName != "renderMode"
				&& memberName != "shaderStages")
			{
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Unknown member '" + memberName + "'.");
			}
		}
	}
	std::string MaterialAssetLoader::GetRequiredString(const nlohmann::json& json, const std::filesystem::path& path, std::string_view memberName)
	{
		// memberName must exist:
		std::string memberNameString(memberName);
		if (!json.contains(memberNameString))
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Missing '" + memberNameString + "'.");

		// memberName value must be a string:
		const nlohmann::json& value = json.at(memberNameString);
		if (!value.is_string())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Member '" + memberNameString + "' must be a string.");

		// memberName value string must not be empty:
		std::string stringValue = value.get<std::string>();
		if (stringValue.empty())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Member '" + memberNameString + "' cannot be empty.");
		return stringValue;
	}
	emberCommon::MaterialPass MaterialAssetLoader::ParseMaterialPass(const std::filesystem::path& path, const std::string& value)
	{
		if (value == "gizmo")
			return emberCommon::MaterialPass::gizmo;
		if (value == "shadow")
			return emberCommon::MaterialPass::shadow;
		if (value == "deferredGeometry")
			return emberCommon::MaterialPass::deferredGeometry;
		if (value == "forward")
			return emberCommon::MaterialPass::forward;

		throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Unsupported material pass '" + value + "'.");
	}
	void MaterialAssetLoader::SetRenderModeSettings(MaterialAsset& materialAsset, const nlohmann::json& json, const std::filesystem::path& path, emberCommon::MaterialPass materialPass)
	{
		bool hasRenderMode = json.contains("renderMode");
		if (hasRenderMode && !json.at("renderMode").is_string())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Member 'renderMode' must be a string.");

		// Extract render mode specific settings:
		switch (materialPass)
		{
			case emberCommon::MaterialPass::gizmo:
			{
				if (!hasRenderMode)
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Gizmo material is missing 'renderMode'.");
				std::string gizmoRenderModeString = GetRequiredString(json, path, "renderMode");
				emberCommon::GizmoRenderMode gizmoRenderMode = ParseGizmoRenderMode(path, gizmoRenderModeString);
				materialAsset.renderModeSettings = MaterialAsset::GizmoSettings{ gizmoRenderMode };
				break;
			}
			case emberCommon::MaterialPass::shadow:
			{
				if (hasRenderMode)
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Shadow material cannot contain 'renderMode'.");
				materialAsset.renderModeSettings = MaterialAsset::ShadowSettings{};
				break;
			}
			case emberCommon::MaterialPass::deferredGeometry:
			{
				if (hasRenderMode)
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Deferred geometry material cannot contain 'renderMode'.");
				materialAsset.renderModeSettings = MaterialAsset::DeferredGeometrySettings{};
				break;
			}
			case emberCommon::MaterialPass::forward:
			{
				if (!hasRenderMode)
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Forward material is missing 'renderMode'.");
				std::string forwardRenderModeString = GetRequiredString(json, path, "renderMode");
				emberCommon::ForwardRenderMode forwardRenderMode = ParseForwardRenderMode(path, forwardRenderModeString);
				materialAsset.renderModeSettings = MaterialAsset::ForwardSettings{ forwardRenderMode };
				break;
			}
			default:
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Unsupported material pass.");
		}
	}
	emberCommon::GizmoRenderMode MaterialAssetLoader::ParseGizmoRenderMode(const std::filesystem::path& path, const std::string& value)
	{
		if (value == "opaque")
			return emberCommon::GizmoRenderMode::opaque;
		if (value == "transparent")
			return emberCommon::GizmoRenderMode::transparent;
		if (value == "wireframe")
			return emberCommon::GizmoRenderMode::wireframe;

		throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Unsupported gizmo render mode '" + value + "'.");
	}
	emberCommon::ForwardRenderMode MaterialAssetLoader::ParseForwardRenderMode(const std::filesystem::path& path, const std::string& value)
	{
		if (value == "opaque")
			return emberCommon::ForwardRenderMode::opaque;
		if (value == "transparent")
			return emberCommon::ForwardRenderMode::transparent;
		if (value == "skybox")
			return emberCommon::ForwardRenderMode::skybox;
		if (value == "wireframe")
			return emberCommon::ForwardRenderMode::wireframe;

		throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Unsupported forward render mode '" + value + "'.");
	}
	void MaterialAssetLoader::SetShaderStages(MaterialAsset& materialAsset, const nlohmann::json& json, const std::filesystem::path& path)
	{
		if (!json.contains("shaderStages"))
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Missing 'shaderStages'.");

		const nlohmann::json& shaderStages = json.at("shaderStages");
		if (!shaderStages.is_object() || shaderStages.empty())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Member 'shaderStages' must be a non-empty object.");

		for (auto iterator = shaderStages.begin(); iterator != shaderStages.end(); iterator++)
		{
			if (!iterator.value().is_string())
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Shader path for stage '" + iterator.key() + "' must be a string.");

			std::string shaderPathString = iterator.value().get<std::string>();
			if (shaderPathString.empty())
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Shader path for stage '" + iterator.key() + "' cannot be empty.");

			emberCommon::ShaderStage shaderStage = ParseShaderStage(path, iterator.key());
			std::filesystem::path shaderPath = shaderPathString;
			if (shaderPath.is_relative())
				shaderPath = path.parent_path() / shaderPath;
			shaderPath = std::filesystem::absolute(shaderPath).lexically_normal().make_preferred();
			if (!std::filesystem::exists(shaderPath))
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Shader file does not exist: " + shaderPath.string());
			if (!std::filesystem::is_regular_file(shaderPath))
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Shader path is not a regular file: " + shaderPath.string());

			materialAsset.shaderStagePaths[static_cast<size_t>(shaderStage)] = std::move(shaderPath);
		}
	}
	emberCommon::ShaderStage MaterialAssetLoader::ParseShaderStage(const std::filesystem::path& path, const std::string& value)
	{
		if (value == "vertex")
			return emberCommon::ShaderStage::vertex;
		if (value == "fragment")
			return emberCommon::ShaderStage::fragment;

		throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Unsupported shader stage '" + value + "'.");
	}
	void MaterialAssetLoader::ValidateShaderStages(const MaterialAsset& materialAsset, const std::filesystem::path& path)
	{
		const std::filesystem::path& vertexPath = materialAsset.shaderStagePaths[static_cast<size_t>(emberCommon::ShaderStage::vertex)];
		const std::filesystem::path& fragmentPath = materialAsset.shaderStagePaths[static_cast<size_t>(emberCommon::ShaderStage::fragment)];
		
		// Vertex shader is always required:
		if (vertexPath.empty())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Missing vertex shader stage.");

		// Shadow shaders can't contain a fragment shader:
		if (materialAsset.GetMaterialPass() == emberCommon::MaterialPass::shadow)
		{
			if (!fragmentPath.empty())
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Shadow materials cannot contain a fragment shader stage.");
			return;
		}

		// Fragment shader required for none shadow shaders:
		if (fragmentPath.empty())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Missing fragment shader stage.");
	}
}