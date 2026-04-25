#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class VertexMemoryLayout : uint32_t
	{
		interleaved,
		separate,
		count
	};
	inline constexpr std::string_view VertexMemoryLayoutToString(VertexMemoryLayout vertexMemoryLayout)
	{
		switch (vertexMemoryLayout)
		{
			case VertexMemoryLayout::interleaved: return "interleaved";
			case VertexMemoryLayout::separate: return "separate";
			case VertexMemoryLayout::count: return "count";
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
	inline constexpr std::string_view RenderModeToString(RenderMode renderMode)
	{
		switch (renderMode)
		{
			case RenderMode::opaque: return "opaque";
			case RenderMode::transparent: return "transparent";
			case RenderMode::skybox: return "skybox";
			case RenderMode::wireframe: return "wireframe";
			case RenderMode::count: return "count";
			default: return "unknown";
		}
	}
}