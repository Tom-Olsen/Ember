#pragma once
#include "commonCullMode.h"
#include "commonRenderQueue.h"
#include "commonRenderStateEnums.h"
#include <cstdint>



namespace emberCommon
{
	struct DeferredGeometryRenderState
	{
	public: // Members:
		// Mutable:
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
		DeferredGeometryRenderState();
		DeferredGeometryRenderState(int32_t renderQueue, CullMode cullMode);
	};

	inline DeferredGeometryRenderState::DeferredGeometryRenderState()
		: DeferredGeometryRenderState(RenderQueue::opaque, CullMode::back)
	{

	}
	inline DeferredGeometryRenderState::DeferredGeometryRenderState(int32_t renderQueue, CullMode cullMode)
		: renderQueue(renderQueue),
		  cullMode(cullMode),
		  polygonMode(PolygonMode::fill),
		  frontFace(FrontFace::counterClockwise),
		  depthTestEnable(true),
		  depthWriteEnable(true),
		  depthCompareOp(CompareOp::less),
		  blendEnable(false),
		  srcColorBlendFactor(BlendFactor::one),
		  dstColorBlendFactor(BlendFactor::zero),
		  colorBlendOp(BlendOp::add),
		  srcAlphaBlendFactor(BlendFactor::one),
		  dstAlphaBlendFactor(BlendFactor::zero),
		  alphaBlendOp(BlendOp::add),
		  colorWriteMask(ColorComponent::all)
	{

	}
}