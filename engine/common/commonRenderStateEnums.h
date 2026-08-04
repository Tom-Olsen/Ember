#pragma once
#include <cstdint>



namespace emberCommon
{
	enum class CompareOp
	{
		never,
		less,
		equal,
		lessOrEqual,
		greater,
		notEqual,
		greaterOrEqual,
		always,
		count
	};

	enum class BlendFactor
	{
		zero,
		one,
		srcColor,
		oneMinusSrcColor,
		dstColor,
		oneMinusDstColor,
		srcAlpha,
		oneMinusSrcAlpha,
		dstAlpha,
		oneMinusDstAlpha,
		count
	};

	enum class BlendOp
	{
		add,
		subtract,
		reverseSubtract,
		min,
		max,
		count
	};

	enum class FrontFace
	{
		clockwise,
		counterClockwise,
		count
	};

	enum class PolygonMode
	{
		fill,
		line,
		point,
		count
	};

	namespace ColorComponent
	{
		inline constexpr uint32_t red = 1 << 0;
		inline constexpr uint32_t green = 1 << 1;
		inline constexpr uint32_t blue = 1 << 2;
		inline constexpr uint32_t alpha = 1 << 3;
		inline constexpr uint32_t all = red | green | blue | alpha;
	}
}