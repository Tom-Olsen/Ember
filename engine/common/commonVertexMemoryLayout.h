#pragma once
#include "emberMath.h"



namespace emberCommon
{
	struct Vertex
	{
		Float3 position;
		Float3 normal;
		Float3 tangent;
		Float4 color;
		Float4 uv;
	};
	enum class VertexMemoryLayout
	{
		interleaved,
		separate
	};
}