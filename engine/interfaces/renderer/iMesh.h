#pragma once
#include "emberMath.h"
#include <vector>
#include <string>



namespace emberBackendInterface
{
    class IMesh
    {
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IMesh() = default;

		// Setters:
		virtual void SetName(const std::string& name) = 0;
		virtual void SetPositions(const std::vector<Float3>& positions) = 0;
		virtual void SetNormals(const std::vector<Float3>& normals) = 0;
		virtual void SetTangents(const std::vector<Float3>& tangents) = 0;
		virtual void SetColors(const std::vector<Float4>& colors) = 0;
		virtual void SetUniformColor(const Float4& color) = 0;
		virtual void SetUVs(const std::vector<Float4>& uvs) = 0;
		virtual void SetTriangles(const std::vector<Uint3>& triangles) = 0;

		// Movers:
		virtual void MovePositions(std::vector<Float3>& positions) = 0;
		virtual void MoveNormals(std::vector<Float3>& normals) = 0;
		virtual void MoveTangents(std::vector<Float3>& tangents) = 0;
		virtual void MoveColors(std::vector<Float4>& colors) = 0;
		virtual void MoveUVs(std::vector<Float4>& uvs) = 0;
		virtual void MoveTriangles(std::vector<Uint3>& triangles) = 0;

		// Getters:
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetVertexCount() const = 0;
		virtual uint32_t GetTriangleCount() const = 0;
		virtual std::vector<Float3>& GetPositions() = 0;
		virtual std::vector<Float3>& GetNormals() = 0;
		virtual std::vector<Float3>& GetTangents() = 0;
		virtual std::vector<Float4>& GetColors() = 0;
		virtual std::vector<Float4>& GetUVs() = 0;
		virtual std::vector<Uint3>& GetTriangles() = 0;

		// Mesh transformations (changes *this):
		virtual void ComputeNormals() = 0;
		virtual void ComputeTangents() = 0;
    };
}