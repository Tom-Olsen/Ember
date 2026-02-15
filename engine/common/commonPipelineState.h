#pragma once
#include "emberMath.h"
#include <string>



namespace emberCommon
{
	// Ember::ToDo: this shouldnt live here.
	struct Vertex
	{
		Float3 position;
		Float3 normal;
		Float3 tangent;
		Float4 color;
		Float4 uv;
	};


	enum class VertexMemoryLayout : uint32_t
	{
		interleaved,
		separate,
		count
	};
	std::string VertexMemoryLayoutToString(VertexMemoryLayout vertexMemoryLayout)
	{
		switch (vertexMemoryLayout)
		{
			case VertexMemoryLayout::interleaved: return "interleaved";
			case VertexMemoryLayout::separate: return "separate";
			default: return "unknown";
		}
	}

	enum class RenderMode : uint32_t
	{
		opaque,
		transparent,
		skybox,
		wireframe,
		count
	};
	std::string RenderModeToString(RenderMode renderMode)
	{
		switch (renderMode)
		{
			case RenderMode::opaque: return "opaque";
			case RenderMode::transparent: return "transparent";
			case RenderMode::skybox: return "skybox";
			case RenderMode::wireframe: return "wireframe";
			default: return "unknown";
		}
	}

	enum class CullMode : uint32_t
	{
		back,
		front,
		none,
		count
	};
	std::string CullModeToString(CullMode cullMode)
	{
		switch (cullMode)
		{
			case CullMode::back: return "back";
			case CullMode::front: return "front";
			case CullMode::none: return "none";
			default: return "unknown";
		}
	}


	struct PipelineState
	{
		VertexMemoryLayout vertexMemoryLayout;
		RenderMode renderMode;
		CullMode cullMode;
		uint32_t Index() const
		{
			constexpr uint32_t vertexMemoryLayoutCount = static_cast<uint32_t>(VertexMemoryLayout::count);
			constexpr uint32_t renderModeCount = static_cast<uint32_t>(RenderMode::count);
			constexpr uint32_t cullModeCount = static_cast<uint32_t>(CullMode::count);
			return static_cast<uint32_t>(vertexMemoryLayout) + static_cast<uint32_t>(renderMode) * vertexMemoryLayoutCount + static_cast<uint32_t>(cullMode) * vertexMemoryLayoutCount * renderModeCount;
		}
		std::string ToString() const
		{
			return "vertexMemoryLayout=" + VertexMemoryLayoutToString(vertexMemoryLayout) + ", renderMode=" + RenderModeToString(renderMode) + ", cullMode=" + CullModeToString(cullMode);
		}
	};
}