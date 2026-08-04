#pragma once
#include "commonCullMode.h"
#include "commonForwardRenderMode.h"
#include "commonRenderQueue.h"
#include "commonRenderStateEnums.h"
#include <cstdint>
#include <stdexcept>



namespace emberCommon
{
	struct ForwardRenderState
	{
	public: // Members:
		// Mutable:
		ForwardRenderMode renderMode;
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
		ForwardRenderState();
		ForwardRenderState(ForwardRenderMode renderMode, int32_t renderQueue, CullMode cullMode, PolygonMode polygonMode, FrontFace frontFace, bool depthTestEnable, bool depthWriteEnable, CompareOp depthCompareOp, bool blendEnable, BlendFactor srcColorBlendFactor, BlendFactor dstColorBlendFactor, BlendOp colorBlendOp, BlendFactor srcAlphaBlendFactor, BlendFactor dstAlphaBlendFactor, BlendOp alphaBlendOp, uint32_t colorWriteMask);

		static ForwardRenderState ForwardDefault(ForwardRenderMode renderMode);
		static ForwardRenderState ForwardDefault(ForwardRenderMode renderMode, int32_t renderQueue);
		static ForwardRenderState OpaqueDefault(int32_t renderQueue = RenderQueue::opaque);
		static ForwardRenderState SkyboxDefault(int32_t renderQueue = RenderQueue::skybox);
		static ForwardRenderState TransparentDefault(int32_t renderQueue = RenderQueue::transparent);
		static ForwardRenderState WireframeDefault(int32_t renderQueue = RenderQueue::opaque);
	};

	inline ForwardRenderState::ForwardRenderState()
		: ForwardRenderState(OpaqueDefault())
	{

	}
	inline ForwardRenderState::ForwardRenderState(ForwardRenderMode renderMode, int32_t renderQueue, CullMode cullMode, PolygonMode polygonMode, FrontFace frontFace, bool depthTestEnable, bool depthWriteEnable, CompareOp depthCompareOp, bool blendEnable, BlendFactor srcColorBlendFactor, BlendFactor dstColorBlendFactor, BlendOp colorBlendOp, BlendFactor srcAlphaBlendFactor, BlendFactor dstAlphaBlendFactor, BlendOp alphaBlendOp, uint32_t colorWriteMask)
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
	inline ForwardRenderState ForwardRenderState::ForwardDefault(ForwardRenderMode renderMode)
	{
		switch (renderMode)
		{
			case ForwardRenderMode::opaque:
				return OpaqueDefault();
			case ForwardRenderMode::transparent:
				return TransparentDefault();
			case ForwardRenderMode::skybox:
				return SkyboxDefault();
			case ForwardRenderMode::wireframe:
				return WireframeDefault();
			default:
				throw std::runtime_error("ForwardRenderState::ForwardDefault(...) failed. Unsupported forward render mode.");
		}
	}
	inline ForwardRenderState ForwardRenderState::ForwardDefault(ForwardRenderMode renderMode, int32_t renderQueue)
	{
		ForwardRenderState renderState = ForwardDefault(renderMode);
		renderState.renderQueue = renderQueue;
		return renderState;
	}
	inline ForwardRenderState ForwardRenderState::OpaqueDefault(int32_t renderQueue)
	{
		return ForwardRenderState(ForwardRenderMode::opaque, renderQueue, CullMode::back, PolygonMode::fill, FrontFace::counterClockwise, true, true, CompareOp::less, false, BlendFactor::srcAlpha, BlendFactor::oneMinusSrcAlpha, BlendOp::add, BlendFactor::one, BlendFactor::zero, BlendOp::add, ColorComponent::all);
	}
	inline ForwardRenderState ForwardRenderState::SkyboxDefault(int32_t renderQueue)
	{
		return ForwardRenderState(ForwardRenderMode::skybox, renderQueue, CullMode::back, PolygonMode::fill, FrontFace::clockwise, true, false, CompareOp::lessOrEqual, false, BlendFactor::srcAlpha, BlendFactor::oneMinusSrcAlpha, BlendOp::add, BlendFactor::one, BlendFactor::zero, BlendOp::add, ColorComponent::all);
	}
	inline ForwardRenderState ForwardRenderState::TransparentDefault(int32_t renderQueue)
	{
		return ForwardRenderState(ForwardRenderMode::transparent, renderQueue, CullMode::none, PolygonMode::fill, FrontFace::counterClockwise, true, false, CompareOp::less, true, BlendFactor::srcAlpha, BlendFactor::oneMinusSrcAlpha, BlendOp::add, BlendFactor::one, BlendFactor::zero, BlendOp::add, ColorComponent::all);
	}
	inline ForwardRenderState ForwardRenderState::WireframeDefault(int32_t renderQueue)
	{
		return ForwardRenderState(ForwardRenderMode::wireframe, renderQueue, CullMode::none, PolygonMode::line, FrontFace::counterClockwise, true, true, CompareOp::less, false, BlendFactor::srcAlpha, BlendFactor::oneMinusSrcAlpha, BlendOp::add, BlendFactor::one, BlendFactor::zero, BlendOp::add, ColorComponent::all);
	}
}