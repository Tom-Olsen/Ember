#pragma once
#include "commonCullMode.h"
#include "commonRenderStateEnums.h"
#include <cstdint>



namespace emberCommon
{
	struct PresentRenderState
	{
	public: // Members:
		// Immutable:
		const CullMode cullMode;
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
		PresentRenderState();
	};

	inline PresentRenderState::PresentRenderState()
		: cullMode(CullMode::front),
		  polygonMode(PolygonMode::fill),
		  frontFace(FrontFace::counterClockwise),
		  depthTestEnable(false),
		  depthWriteEnable(false),
		  depthCompareOp(CompareOp::always),
		  blendEnable(false),
		  srcColorBlendFactor(BlendFactor::srcAlpha),
		  dstColorBlendFactor(BlendFactor::oneMinusSrcAlpha),
		  colorBlendOp(BlendOp::add),
		  srcAlphaBlendFactor(BlendFactor::one),
		  dstAlphaBlendFactor(BlendFactor::zero),
		  alphaBlendOp(BlendOp::add),
		  colorWriteMask(ColorComponent::all)
	{

	}
}