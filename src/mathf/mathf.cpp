#include "mathf.h"
#include <math.h>



namespace mathf
{
	float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
	float ToDegrees(float radians)
	{
		return radians * 180.0f / PI;
	}
	float ToRadians(float degrees)
	{
		return degrees * PI / 180.0f;
	}
}