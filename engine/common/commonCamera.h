#pragma once
#include "emberMath.h"



namespace emberCommon
{
	struct Camera
	{
		Float3 position;
		Float4x4 viewMatrix;
		Float4x4 projectionMatrix;
	};
}