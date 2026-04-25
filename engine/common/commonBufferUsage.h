#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class BufferUsage : uint32_t
	{
		index,
		vertex,
		storage
	};
	inline constexpr std::string_view BufferUsageToString(BufferUsage usage)
	{
		switch (usage)
		{
			case BufferUsage::index: return "index";
			case BufferUsage::vertex: return "vertex";
			case BufferUsage::storage: return "storage";
			default: return "unknown";
		}
	}
}