#pragma once
#include <cstdint>



namespace emberCommon
{
	enum class ShaderStage : uint32_t
	{
		vertex,
		fragment,
		// add more later.
		count
	};
}