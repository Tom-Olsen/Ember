#pragma once
#include "emberMath.h"
#include <string>
#include <sstream>
#include <string_view>



namespace emberCommon
{
	enum class ShadowType
	{
		hard,
		soft
	};
	inline constexpr std::string_view ShadowTypeToString(ShadowType shadowType)
	{
		switch (shadowType)
		{
			case ShadowType::hard: return "hard";
			case ShadowType::soft: return "soft";
			default: return "unknown";
		}
	}

	struct DirectionalLight
	{
		Float3 direction;
		float intensity;
		Float3 color;
		ShadowType shadowType;
		Float4x4 worldToClipMatrix;
		inline std::string ToString() const
		{
			std::ostringstream ss;
			ss << "direction: " << direction.ToString() << "\n";
			ss << "intensity: " << intensity << "\n";
			ss << "color: " << color.ToString() << "\n";
			ss << "shadowType: " << ShadowTypeToString(shadowType) << "\n";
			ss << "worldToClipMatrix: " << worldToClipMatrix.ToString();
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
		inline std::string ToString() const
		{
			std::ostringstream ss;
			ss << "position: " << position.ToString() << "\n";
			ss << "intensity: " << intensity << "\n";
			ss << "color: " << color.ToString() << "\n";
			ss << "shadowType: " << ShadowTypeToString(shadowType) << "\n";
			ss << "blendStart: " << blendStart << "\n";
			ss << "blendEnd: " << blendEnd << "\n";
			ss << "worldToClipMatrix: " << worldToClipMatrix.ToString();
			return ss.str();
		}
	};
}