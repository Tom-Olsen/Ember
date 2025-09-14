#pragma once
#include "emberMath.h"
#include <memory>
#include <string>
#include <vector>



// Forward decleration:
namespace emberBackendInterface
{
	class IMesh;
}



namespace emberEngine
{
	class Mesh
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::IMesh> m_pIMesh;

	public: // Constructors/Destructor:
		Mesh(const std::string& name = "");
		~Mesh();

	public: // Setters:
		void SetName(const std::string& name);
		void SetPositions(const std::vector<Float3>& positions);
		void SetNormals(const std::vector<Float3>& normals);
		void SetTangents(const std::vector<Float3>& tangents);
		void SetColors(const std::vector<Float4>& colors);
		void SetUniformColor(const Float4& color);
		void SetUVs(const std::vector<Float4>& uvs);
		void SetTriangles(const std::vector<Uint3>& triangles);

	public: // Movers:
		void MovePositions(std::vector<Float3>& positions);
		void MoveNormals(std::vector<Float3>& normals);
		void MoveTangents(std::vector<Float3>& tangents);
		void MoveColors(std::vector<Float4>& colors);
		void MoveUVs(std::vector<Float4>& uvs);
		void MoveTriangles(std::vector<Uint3>& triangles);

	public: // Getters:
		const std::string& GetName() const;
		uint32_t GetVertexCount() const;
		uint32_t GetTriangleCount() const;
		std::vector<Float3>& GetPositions();
		std::vector<Float3>& GetNormals();
		std::vector<Float3>& GetTangents();
		std::vector<Float4>& GetColors();
		std::vector<Float4>& GetUVs();
		std::vector<Uint3>& GetTriangles();

	public: // Mesh transformations (changes *this):
		void ComputeNormals();
		void ComputeTangents();
	};
}