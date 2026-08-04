#pragma once
#include "commonCullMode.h"
#include "commonGizmoRenderMode.h"
#include "commonRenderQueue.h"
#include "commonRenderStateEnums.h"
#include <cstdint>
#include <stdexcept>



namespace emberCommon
{
	struct GizmoRenderState
	{
	public: // Members:
		// Mutable:
		GizmoRenderMode renderMode;
		int32_t renderQueue;
		CullMode cullMode;
		// Immutable:
		const PolygonMode polygonMode;
		const FrontFace frontFace;
		const bool depthTestEnable;
		const bool depthWriteEnable;
		const CompareOp depthCompareOp;
		const bool blendEnable;
		const BlendFactor srcColorBlendFactor;
		const BlendFactor dstColorBlendFactor;
		const BlendOp colorBlendOp;
		const BlendFactor srcAlphaBlendFactor;
		const BlendFactor dstAlphaBlendFactor;
		const BlendOp alphaBlendOp;
		const uint32_t colorWriteMask;

	public: // Methods:
		GizmoRenderState();
		GizmoRenderState(GizmoRenderMode renderMode, int32_t renderQueue, CullMode cullMode, PolygonMode polygonMode, FrontFace frontFace, bool depthTestEnable, bool depthWriteEnable, CompareOp depthCompareOp, bool blendEnable, BlendFactor srcColorBlendFactor, BlendFactor dstColorBlendFactor, BlendOp colorBlendOp, BlendFactor srcAlphaBlendFactor, BlendFactor dstAlphaBlendFactor, BlendOp alphaBlendOp, uint32_t colorWriteMask);

		static GizmoRenderState GizmoDefault(GizmoRenderMode renderMode);
		static GizmoRenderState GizmoDefault(GizmoRenderMode renderMode, int32_t renderQueue);
		static GizmoRenderState OpaqueDefault(int32_t renderQueue = RenderQueue::opaque);
		static GizmoRenderState TransparentDefault(int32_t renderQueue = RenderQueue::transparent);
		static GizmoRenderState WireframeDefault(int32_t renderQueue = RenderQueue::opaque);
	};

	inline GizmoRenderState::GizmoRenderState()
		: GizmoRenderState(OpaqueDefault())
	{

	}
	inline GizmoRenderState::GizmoRenderState(GizmoRenderMode renderMode, int32_t renderQueue, CullMode cullMode, PolygonMode polygonMode, FrontFace frontFace, bool depthTestEnable, bool depthWriteEnable, CompareOp depthCompareOp, bool blendEnable, BlendFactor srcColorBlendFactor, BlendFactor dstColorBlendFactor, BlendOp colorBlendOp, BlendFactor srcAlphaBlendFactor, BlendFactor dstAlphaBlendFactor, BlendOp alphaBlendOp, uint32_t colorWriteMask)
		: renderMode(renderMode),
		  renderQueue(renderQueue),
		  cullMode(cullMode),
		  polygonMode(polygonMode),
		  frontFace(frontFace),
		  depthTestEnable(depthTestEnable),
		  depthWriteEnable(depthWriteEnable),
		  depthCompareOp(depthCompareOp),
		  blendEnable(blendEnable),
		  srcColorBlendFactor(srcColorBlendFactor),
		  dstColorBlendFactor(dstColorBlendFactor),
		  colorBlendOp(colorBlendOp),
		  srcAlphaBlendFactor(srcAlphaBlendFactor),
		  dstAlphaBlendFactor(dstAlphaBlendFactor),
		  alphaBlendOp(alphaBlendOp),
		  colorWriteMask(colorWriteMask)
	{

	}
	inline GizmoRenderState GizmoRenderState::GizmoDefault(GizmoRenderMode renderMode)
	{
		switch (renderMode)
		{
			case GizmoRenderMode::opaque:
				return OpaqueDefault();
			case GizmoRenderMode::transparent:
				return TransparentDefault();
			case GizmoRenderMode::wireframe:
				return WireframeDefault();
			default:
				throw std::runtime_error("GizmoRenderState::GizmoDefault(...) failed. Unsupported gizmo render mode.");
		}
	}
	inline GizmoRenderState GizmoRenderState::GizmoDefault(GizmoRenderMode renderMode, int32_t renderQueue)
	{
		GizmoRenderState renderState = GizmoDefault(renderMode);
		renderState.renderQueue = renderQueue;
		return renderState;
	}
	inline GizmoRenderState GizmoRenderState::OpaqueDefault(int32_t renderQueue)
	{
		return GizmoRenderState(GizmoRenderMode::opaque, renderQueue, CullMode::back, PolygonMode::fill, FrontFace::counterClockwise, true, true, CompareOp::less, false, BlendFactor::srcAlpha, BlendFactor::oneMinusSrcAlpha, BlendOp::add, BlendFactor::one, BlendFactor::zero, BlendOp::add, ColorComponent::all);
	}
	inline GizmoRenderState GizmoRenderState::TransparentDefault(int32_t renderQueue)
	{
		return GizmoRenderState(GizmoRenderMode::transparent, renderQueue, CullMode::none, PolygonMode::fill, FrontFace::counterClockwise, true, false, CompareOp::less, true, BlendFactor::srcAlpha, BlendFactor::oneMinusSrcAlpha, BlendOp::add, BlendFactor::one, BlendFactor::zero, BlendOp::add, ColorComponent::all);
	}
	inline GizmoRenderState GizmoRenderState::WireframeDefault(int32_t renderQueue)
	{
		return GizmoRenderState(GizmoRenderMode::wireframe, renderQueue, CullMode::none, PolygonMode::line, FrontFace::counterClockwise, true, true, CompareOp::less, false, BlendFactor::srcAlpha, BlendFactor::oneMinusSrcAlpha, BlendOp::add, BlendFactor::one, BlendFactor::zero, BlendOp::add, ColorComponent::all);
	}
}