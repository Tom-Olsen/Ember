#pragma once
#include "commonCullMode.h"
#include "commonPipelineState.h"
#include "commonRenderQueue.h"
#include <cstdint>
#include <stdexcept>



namespace emberCommon
{
	enum class CompareOp
	{
		never,
		less,
		equal,
		lessOrEqual,
		greater,
		notEqual,
		greaterOrEqual,
		always,
		count
	};

	enum class BlendFactor
	{
		zero,
		one,
		srcColor,
		oneMinusSrcColor,
		dstColor,
		oneMinusDstColor,
		srcAlpha,
		oneMinusSrcAlpha,
		dstAlpha,
		oneMinusDstAlpha,
		count
	};

	enum class BlendOp
	{
		add,
		subtract,
		reverseSubtract,
		min,
		max,
		count
	};

	enum class FrontFace
	{
		clockwise,
		counterClockwise,
		count
	};

	enum class PolygonMode
	{
		fill,
		line,
		point,
		count
	};

	namespace ColorComponent
	{
		inline constexpr uint32_t red = 1 << 0;
		inline constexpr uint32_t green = 1 << 1;
		inline constexpr uint32_t blue = 1 << 2;
		inline constexpr uint32_t alpha = 1 << 3;
		inline constexpr uint32_t all = red | green | blue | alpha;
	}

	struct MaterialRenderState
	{
	public: // Members:
		RenderMode renderMode;
		int32_t renderQueue;
		CullMode cullMode;
		PolygonMode polygonMode;
		FrontFace frontFace;
		bool depthTestEnable;
		bool depthWriteEnable;
		CompareOp depthCompareOp;
		bool blendEnable;
		BlendFactor srcColorBlendFactor;
		BlendFactor dstColorBlendFactor;
		BlendOp colorBlendOp;
		BlendFactor srcAlphaBlendFactor;
		BlendFactor dstAlphaBlendFactor;
		BlendOp alphaBlendOp;
		uint32_t colorWriteMask;

	public: // Methods:
		// Constructors:
		MaterialRenderState();
		MaterialRenderState(RenderMode renderMode);
		MaterialRenderState(RenderMode renderMode, int32_t renderQueue);

		// Presets:
		static MaterialRenderState DefaultForRenderMode(RenderMode renderMode);
		static MaterialRenderState DefaultForRenderMode(RenderMode renderMode, int32_t renderQueue);
		static MaterialRenderState OpaqueDefault(int32_t renderQueue = RenderQueue::opaque);
		static MaterialRenderState OutlineDefault(int32_t renderQueue = RenderQueue::opaque);
		static MaterialRenderState PresentDefault(int32_t renderQueue = RenderQueue::opaque);
		static MaterialRenderState ShadowDefault(int32_t renderQueue = RenderQueue::opaque);
		static MaterialRenderState SkyboxDefault(int32_t renderQueue = RenderQueue::skybox);
		static MaterialRenderState TransparentDefault(int32_t renderQueue = RenderQueue::transparent);
		static MaterialRenderState WireframeDefault(int32_t renderQueue = RenderQueue::opaque);
	};

	inline MaterialRenderState::MaterialRenderState()
		: renderMode(RenderMode::opaque),
		  renderQueue(RenderQueue::opaque),
		  cullMode(CullMode::back),
		  polygonMode(PolygonMode::fill),
		  frontFace(FrontFace::counterClockwise),
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
	inline MaterialRenderState::MaterialRenderState(RenderMode renderMode)
		: MaterialRenderState(DefaultForRenderMode(renderMode))
	{

	}
	inline MaterialRenderState::MaterialRenderState(RenderMode renderMode, int32_t renderQueue)
		: MaterialRenderState(DefaultForRenderMode(renderMode, renderQueue))
	{

	}
	inline MaterialRenderState MaterialRenderState::DefaultForRenderMode(RenderMode renderMode)
	{
		switch (renderMode)
		{
			case RenderMode::opaque:
				return OpaqueDefault();
			case RenderMode::transparent:
				return TransparentDefault();
			case RenderMode::skybox:
				return SkyboxDefault();
			case RenderMode::wireframe:
				return WireframeDefault();
			default:
				throw std::runtime_error("MaterialRenderState::DefaultForRenderMode(...) failed. Unsupported render mode.");
		}
	}
	inline MaterialRenderState MaterialRenderState::DefaultForRenderMode(RenderMode renderMode, int32_t renderQueue)
	{
		MaterialRenderState renderState = DefaultForRenderMode(renderMode);
		renderState.renderQueue = renderQueue;
		return renderState;
	}
	inline MaterialRenderState MaterialRenderState::OpaqueDefault(int32_t renderQueue)
	{
		MaterialRenderState renderState;
		renderState.renderMode = RenderMode::opaque;
		renderState.renderQueue = renderQueue;
		renderState.cullMode = CullMode::back;
		renderState.polygonMode = PolygonMode::fill;
		renderState.frontFace = FrontFace::counterClockwise;
		renderState.depthTestEnable = true;
		renderState.depthWriteEnable = true;
		renderState.depthCompareOp = CompareOp::less;
		renderState.blendEnable = false;
		renderState.srcColorBlendFactor = BlendFactor::srcAlpha;
		renderState.dstColorBlendFactor = BlendFactor::oneMinusSrcAlpha;
		renderState.colorBlendOp = BlendOp::add;
		renderState.srcAlphaBlendFactor = BlendFactor::one;
		renderState.dstAlphaBlendFactor = BlendFactor::zero;
		renderState.alphaBlendOp = BlendOp::add;
		renderState.colorWriteMask = ColorComponent::all;
		return renderState;
	}
	inline MaterialRenderState MaterialRenderState::OutlineDefault(int32_t renderQueue)
	{
		MaterialRenderState renderState = OpaqueDefault(renderQueue);
		renderState.cullMode = CullMode::none;
		renderState.depthTestEnable = false;
		renderState.depthWriteEnable = false;
		renderState.depthCompareOp = CompareOp::always;
		renderState.colorWriteMask = ColorComponent::red;
		return renderState;
	}
	inline MaterialRenderState MaterialRenderState::PresentDefault(int32_t renderQueue)
	{
		MaterialRenderState renderState = OpaqueDefault(renderQueue);
		renderState.cullMode = CullMode::front;
		renderState.depthTestEnable = false;
		renderState.depthWriteEnable = false;
		renderState.depthCompareOp = CompareOp::always;
		return renderState;
	}
	inline MaterialRenderState MaterialRenderState::ShadowDefault(int32_t renderQueue)
	{
		MaterialRenderState renderState = OpaqueDefault(renderQueue);
		renderState.cullMode = CullMode::none;
		renderState.frontFace = FrontFace::clockwise;
		return renderState;
	}
	inline MaterialRenderState MaterialRenderState::SkyboxDefault(int32_t renderQueue)
	{
		MaterialRenderState renderState = OpaqueDefault(renderQueue);
		renderState.renderMode = RenderMode::skybox;
		renderState.frontFace = FrontFace::clockwise;
		renderState.depthWriteEnable = false;
		renderState.depthCompareOp = CompareOp::lessOrEqual;
		return renderState;
	}
	inline MaterialRenderState MaterialRenderState::TransparentDefault(int32_t renderQueue)
	{
		MaterialRenderState renderState = OpaqueDefault(renderQueue);
		renderState.renderMode = RenderMode::transparent;
		renderState.cullMode = CullMode::none;
		renderState.depthWriteEnable = false;
		renderState.blendEnable = true;
		return renderState;
	}
	inline MaterialRenderState MaterialRenderState::WireframeDefault(int32_t renderQueue)
	{
		MaterialRenderState renderState = OpaqueDefault(renderQueue);
		renderState.renderMode = RenderMode::wireframe;
		renderState.cullMode = CullMode::none;
		renderState.polygonMode = PolygonMode::line;
		return renderState;
	}
}