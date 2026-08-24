#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialPass.h"
#include <array>
#include <cstddef>
#include <cstdint>



namespace vulkanRendererBackend
{
	enum class RenderStage : uint8_t
	{
		resourceUpdate,
		gizmo,
		preRenderCompute,
		outline,
		shadow,
		forward,
		postRenderCompute,
		present,
		stageCount
	};
	inline static constexpr std::array<const char*, static_cast<size_t>(RenderStage::stageCount)> renderStageNames =
	{
		"resourceUpdate",
		"gizmo",
		"preRenderCompute",
		"outline",
		"shadow",
		"forward",
		"postRenderCompute",
		"present"
	};


	
	template<RenderStage stage>
	struct RenderStageTraits
	{
		static constexpr bool hasRenderPipeline = false;
		static constexpr bool hasRenderMode = false;
		using RenderMode = void;
	};

	template<>
	struct RenderStageTraits<RenderStage::gizmo>
	{
		static constexpr bool hasRenderPipeline = true;
		static constexpr bool hasRenderMode = true;
		using RenderMode = emberCommon::GizmoRenderMode;
		static constexpr emberCommon::MaterialPass materialPass = emberCommon::MaterialPass::gizmo;

		static constexpr uint32_t RenderModeIndex(RenderMode renderMode)
		{
			return static_cast<uint32_t>(renderMode);
		}
	};

	template<>
	struct RenderStageTraits<RenderStage::outline>
	{
		static constexpr bool hasRenderPipeline = true;
		static constexpr bool hasRenderMode = false;
		using RenderMode = void;
		static constexpr emberCommon::MaterialPass materialPass = emberCommon::MaterialPass::outline;

		static constexpr uint32_t RenderModeIndex()
		{
			return 0;
		}
	};

	template<>
	struct RenderStageTraits<RenderStage::shadow>
	{
		static constexpr bool hasRenderPipeline = true;
		static constexpr bool hasRenderMode = false;
		using RenderMode = void;
		static constexpr emberCommon::MaterialPass materialPass = emberCommon::MaterialPass::shadow;

		static constexpr uint32_t RenderModeIndex()
		{
			return 0;
		}
	};

	template<>
	struct RenderStageTraits<RenderStage::forward>
	{
		static constexpr bool hasRenderPipeline = true;
		static constexpr bool hasRenderMode = true;
		using RenderMode = emberCommon::ForwardRenderMode;
		static constexpr emberCommon::MaterialPass materialPass = emberCommon::MaterialPass::forward;

		static constexpr uint32_t RenderModeIndex(RenderMode renderMode)
		{
			return static_cast<uint32_t>(renderMode);
		}
	};

	template<>
	struct RenderStageTraits<RenderStage::present>
	{
		static constexpr bool hasRenderPipeline = true;
		static constexpr bool hasRenderMode = false;
		using RenderMode = void;
		static constexpr emberCommon::MaterialPass materialPass = emberCommon::MaterialPass::present;

		static constexpr uint32_t RenderModeIndex()
		{
			return 0;
		}
	};



	// Concepts:
	template<RenderStage stage>
	concept HasRenderPipeline = RenderStageTraits<stage>::hasRenderPipeline;
	template<RenderStage stage>
	concept HasRenderPipelineAndMode = HasRenderPipeline<stage> && RenderStageTraits<stage>::hasRenderMode;
	template<RenderStage stage>
	concept HasRenderPipelineAndNotMode = HasRenderPipeline<stage> && !RenderStageTraits<stage>::hasRenderMode;
}