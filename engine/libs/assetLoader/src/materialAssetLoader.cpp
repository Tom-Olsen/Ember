#include "materialAssetLoader.h"
#include "json.h"
#include "jsonUtility.h"
#include <stdexcept>
#include <string>
#include <utility>



namespace emberAssetLoader
{
	// Public methods:
	MaterialAsset MaterialAssetLoader::Load(const std::filesystem::path& path)
	{
		nlohmann::json json = JsonUtility::LoadObject(path);
		ValidateRootMembers(json, path);
		MaterialAsset materialAsset = {};
		materialAsset.materialName = JsonUtility::GetRequiredString(json, path, "materialName");
		materialAsset.materialShaderName = JsonUtility::GetRequiredString(json, path, "materialShaderName");
		emberCommon::MaterialPass materialPass = ParseMaterialPass(path, JsonUtility::GetRequiredString(json, path, "materialPass"));
		SetRenderModeSettings(materialAsset, json, path, materialPass);
		SetShaderStages(materialAsset, json, path);
		ValidateShaderStages(materialAsset, path);
		materialAsset.accessRights = JsonUtility::GetResourceAccessRights(json, path);
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
				&& memberName != "shaderStages"
				&& memberName != "accessRights")
			{
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Unknown member '" + memberName + "'.");
			}
		}
	}
	emberCommon::MaterialPass MaterialAssetLoader::ParseMaterialPass(const std::filesystem::path& path, const std::string& value)
	{
		if (value == "gizmo")
			return emberCommon::MaterialPass::gizmo;
		if (value == "outline")
			return emberCommon::MaterialPass::outline;
		if (value == "shadow")
			return emberCommon::MaterialPass::shadow;
		if (value == "deferredGeometry")
			return emberCommon::MaterialPass::deferredGeometry;
		if (value == "deferredLighting")
			return emberCommon::MaterialPass::deferredLighting;
		if (value == "forward")
			return emberCommon::MaterialPass::forward;
		if (value == "present")
			return emberCommon::MaterialPass::present;

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
				std::string gizmoRenderModeString = JsonUtility::GetRequiredString(json, path, "renderMode");
				emberCommon::GizmoRenderMode gizmoRenderMode = ParseGizmoRenderMode(path, gizmoRenderModeString);
				materialAsset.renderModeSettings = MaterialAsset::GizmoSettings{ gizmoRenderMode };
				break;
			}
			case emberCommon::MaterialPass::outline:
			{
				if (hasRenderMode)
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Outline material cannot contain 'renderMode'.");
				materialAsset.renderModeSettings = MaterialAsset::OutlineSettings{};
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
			case emberCommon::MaterialPass::deferredLighting:
			{
				if (hasRenderMode)
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Deferred lighting material cannot contain 'renderMode'.");
				materialAsset.renderModeSettings = MaterialAsset::DeferredLightingSettings{};
				break;
			}
			case emberCommon::MaterialPass::forward:
			{
				if (!hasRenderMode)
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Forward material is missing 'renderMode'.");
				std::string forwardRenderModeString = JsonUtility::GetRequiredString(json, path, "renderMode");
				emberCommon::ForwardRenderMode forwardRenderMode = ParseForwardRenderMode(path, forwardRenderModeString);
				materialAsset.renderModeSettings = MaterialAsset::ForwardSettings{ forwardRenderMode };
				break;
			}
			case emberCommon::MaterialPass::present:
			{
				if (hasRenderMode)
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Present material cannot contain 'renderMode'.");
				materialAsset.renderModeSettings = MaterialAsset::PresentSettings{};
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
			emberCommon::ShaderStage shaderStage = ParseShaderStage(path, iterator.key());
			const nlohmann::json& shaderStageJson = iterator.value();
			if (!shaderStageJson.is_object())
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Shader stage '" + iterator.key() + "' must be an object.");
			for (auto memberIterator = shaderStageJson.begin(); memberIterator != shaderStageJson.end(); memberIterator++)
				if (memberIterator.key() != "sourcePath" && memberIterator.key() != "binaryPath")
					throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Unknown member '" + memberIterator.key() + "' in shader stage '" + iterator.key() + "'.");

			MaterialAsset::ShaderStageAsset& shaderStageAsset = materialAsset.shaderStages[static_cast<size_t>(shaderStage)];
			shaderStageAsset.shaderStage = shaderStage;
			shaderStageAsset.sourcePath = JsonUtility::ResolveFilePath(path, JsonUtility::GetRequiredString(shaderStageJson, path, "sourcePath"), "sourcePath");
			shaderStageAsset.binaryPath = JsonUtility::ResolveFilePath(path, JsonUtility::GetRequiredString(shaderStageJson, path, "binaryPath"), "binaryPath");
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
		const MaterialAsset::ShaderStageAsset& vertexStage = materialAsset.shaderStages[static_cast<size_t>(emberCommon::ShaderStage::vertex)];
		const MaterialAsset::ShaderStageAsset& fragmentStage = materialAsset.shaderStages[static_cast<size_t>(emberCommon::ShaderStage::fragment)];
		
		// Vertex shader is always required:
		if (vertexStage.sourcePath.empty() || vertexStage.binaryPath.empty())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Missing vertex shader stage.");

		// Shadow shaders can't contain a fragment shader:
		if (materialAsset.GetMaterialPass() == emberCommon::MaterialPass::shadow)
		{
			if (!fragmentStage.sourcePath.empty() || !fragmentStage.binaryPath.empty())
				throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Shadow materials cannot contain a fragment shader stage.");
			return;
		}

		// Fragment shader required for none shadow shaders:
		if (fragmentStage.sourcePath.empty() || fragmentStage.binaryPath.empty())
			throw std::runtime_error("MaterialAssetLoader::Load(...) failed for '" + path.string() + "'. Missing fragment shader stage.");
	}
}