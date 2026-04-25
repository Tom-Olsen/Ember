#pragma once
#include "emberMath.h"
#include <sstream>
#include <string>



namespace emberCommon
{
	struct Camera
	{
		Float3 position;
		Float4x4 viewMatrix;
		Float4x4 projectionMatrix;

		inline std::string ToString() const
		{
			std::ostringstream ss;
			ss << "position: " << position.ToString() << "\n";
			ss << "viewMatrix: " << viewMatrix.ToString() << "\n";
			ss << "projectionMatrix: " << projectionMatrix.ToString();
			return ss.str();
		}
	};
}