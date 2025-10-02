#pragma once
#include <cstdint>



namespace emberCommon
{
	enum RenderQueue : int32_t // negative values allowed.
	{
		shadow = 0,
		opaque = 1000,
		skybox = 2000,
		transparent = 3000
	};
}