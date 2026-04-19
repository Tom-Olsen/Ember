#pragma once
#include <cstdint>



namespace emberCommon
{
	enum RenderQueue : int32_t // negative values allowed.
	{
		opaque = 0,
		skybox = 1000,
		transparent = 2000
	};
}