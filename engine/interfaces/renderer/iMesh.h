#pragma once
#include "emberMath.h"
#include <vector>



namespace emberBackendInterface
{
    class IMesh
    {
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IMesh() = default;

		// Update GPU buffers:
		virtual void UpdateVertexBuffer(std::vector<Float3>* positions, std::vector<Float3>* normals, std::vector<Float3>* tangents, std::vector<Float4>* colors, std::vector<Float4>* uvs);
		virtual void UpdateIndexBuffer(std::vector<Uint3>* triangles);
    };
}