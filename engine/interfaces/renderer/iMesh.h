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

		// Setters:
		//virtual void SetMeshType(MeshType type);

		// Getters:
		//virtual MeshType GetMeshType() const;
		virtual emberCommon::VertexMemoryLayout GetVertexMemoryLayout() const = 0;

		// Update GPU buffers:
		virtual void UpdateVertexBuffer(const std::vector<Float3>& positions, std::vector<Float3>* pNormals, std::vector<Float3>* pTangents, std::vector<Float4>* pColors, std::vector<Float4>* pUvs, emberCommon::VertexMemoryLayout vertexMemoryLayout) = 0;
		virtual void UpdateVertexBuffer(const std::vector<Float3>& positions, std::vector<Float3>* pNormals, std::vector<Float3>* pTangents, std::vector<Float4>* pColors, std::vector<Float4>* pUvs) = 0;
		virtual void UpdateIndexBuffer(const std::vector<Uint3>& triangles, uint32_t vertexCount = 0) = 0;
    };
}