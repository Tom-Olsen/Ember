#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class VertexMemoryLayout : uint32_t
	{
		interleaved,
		separate,
		vertexMemoryLayoutCount,
		none,
		count
	};
	inline constexpr std::string_view VertexMemoryLayoutToString(VertexMemoryLayout vertexMemoryLayout)
	{
		switch (vertexMemoryLayout)
		{
			case VertexMemoryLayout::interleaved: return "interleaved";
			case VertexMemoryLayout::separate: return "separate";
			case VertexMemoryLayout::vertexMemoryLayoutCount: return "vertexMemoryLayoutCount";
			case VertexMemoryLayout::none: return "none";
			case VertexMemoryLayout::count: return "count";
			default: return "unknown";
		}
	}
}