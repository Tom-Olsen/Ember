#pragma once
#include "emberMath.h"
#include <string>
#include <sstream>



namespace emberCommon
{
	enum class ShadowType
	{
		hard,
		soft
	};

	struct DirectionalLight
	{
		Float3 direction;
		float intensity;
		Float3 color;
		ShadowType shadowType;
		Float4x4 worldToClipMatrix;
		inline std::string ToString()
		{
			std::ostringstream ss;
			ss << "direction: " << direction.ToString() << "\n";
			ss << "intensity: " << intensity << "\n";
			ss << "color: " << color.ToString() << "\n";
			ss << "shadowType: " << (shadowType == ShadowType::hard ? "hard" : "soft") << "\n";
			ss << "worldToClipMatrix: " << worldToClipMatrix.ToString() << "\n";
			return ss.str();
		}
	};

	struct PositionalLight
	{
		Float3 position;
		float intensity;
		Float3 color;
		ShadowType shadowType;
		float blendStart;
		float blendEnd;
		Float4x4 worldToClipMatrix;
		inline std::string ToString()
		{
			std::ostringstream ss;
			ss << "position: " << position.ToString() << "\n";
			ss << "intensity: " << intensity << "\n";
			ss << "color: " << color.ToString() << "\n";
			ss << "shadowType: " << (shadowType == ShadowType::hard ? "hard" : "soft") << "\n";
			ss << "blendStart: " << blendStart << "\n";
			ss << "blendEnd: " << blendEnd << "\n";
			ss << "worldToClipMatrix: " << worldToClipMatrix.ToString() << "\n";
			return ss.str();
		}
	};
}