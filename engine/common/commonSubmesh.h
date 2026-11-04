#pragma once
#include "bounds.h"
#include <cstdint>



namespace emberCommon
{
	struct Submesh
	{
	public: // Members:
		static constexpr uint32_t entireMesh = UINT32_MAX;
		uint32_t firstVertex = 0;
		uint32_t vertexCount = 0;
		uint32_t firstTriangle = 0;
		uint32_t triangleCount = 0;
		emberMath::Bounds bounds;
	};
}