#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialPass.h"
#include "commonResourceAccessRights.h"
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
			std::filesystem::path sourcePath;
			std::filesystem::path binaryPath;
		};
		struct GizmoSettings
		{
			emberCommon::GizmoRenderMode renderMode = emberCommon::GizmoRenderMode::count;
		};
		struct ShadowSettings
		{

		};
		struct OutlineSettings
		{

		};
		struct DeferredGeometrySettings
		{

		};
		struct DeferredLightingSettings
		{

		};
		struct ForwardSettings
		{
			emberCommon::ForwardRenderMode renderMode = emberCommon::ForwardRenderMode::count;
		};
		struct PresentSettings
		{

		};
		using RenderModeSettings = std::variant<
			std::monostate,
			GizmoSettings,
			OutlineSettings,
			ShadowSettings,
			DeferredGeometrySettings,
			DeferredLightingSettings,
			ForwardSettings,
			PresentSettings>;

		// Members:
		std::string materialName;
		std::string materialShaderName;
		std::array<ShaderStageAsset, static_cast<size_t>(emberCommon::ShaderStage::count)> shaderStages;
		RenderModeSettings renderModeSettings = std::monostate{};
		emberCommon::ResourceAccessRights accessRights;

		// Methods:
		emberCommon::MaterialPass GetMaterialPass() const;
	};
}