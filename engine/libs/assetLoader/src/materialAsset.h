#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialPass.h"
#include "commonShaderStage.h"
#include <array>
#include <cstddef>
#include <filesystem>
#include <string>
#include <variant>



namespace emberAssetLoader
{
	struct MaterialAsset
	{
		// Structs:
		struct ShaderStageAsset
		{
			emberCommon::ShaderStage shaderStage = emberCommon::ShaderStage::count;
			std::filesystem::path path;
		};
		struct GizmoSettings
		{
			emberCommon::GizmoRenderMode renderMode = emberCommon::GizmoRenderMode::count;
		};
		struct ShadowSettings
		{

		};
		struct DeferredGeometrySettings
		{

		};
		struct ForwardSettings
		{
			emberCommon::ForwardRenderMode renderMode = emberCommon::ForwardRenderMode::count;
		};
		using RenderModeSettings = std::variant<
			std::monostate,
			GizmoSettings,
			ShadowSettings,
			DeferredGeometrySettings,
			ForwardSettings>;

		// Members:
		std::string materialName;
		std::string materialShaderName;
		std::array<std::filesystem::path, static_cast<size_t>(emberCommon::ShaderStage::count)> shaderStagePaths;
		RenderModeSettings renderModeSettings = std::monostate{};

		// Methods:
		emberCommon::MaterialPass GetMaterialPass() const;
	};
}