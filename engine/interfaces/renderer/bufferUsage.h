#pragma once
#include <string_view>



namespace emberEngine
{
	enum class BufferUsage : uint32_t
	{
		index,
		vertex,
		storage
	};
	constexpr std::string_view BufferUsageNames[] =
	{
		"index",
		"vertex",
		"storage"
	};
}