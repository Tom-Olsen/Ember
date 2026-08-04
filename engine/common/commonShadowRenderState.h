#pragma once
#include "commonCullMode.h"
#include "commonRenderStateEnums.h"
#include <cstdint>



namespace emberCommon
{
	struct ShadowRenderState
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
		ShadowRenderState();
	};

	inline ShadowRenderState::ShadowRenderState()
		: cullMode(CullMode::none),
		  polygonMode(PolygonMode::fill),
		  frontFace(FrontFace::clockwise),
		  depthTestEnable(true),
		  depthWriteEnable(true),
		  depthCompareOp(CompareOp::less),
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