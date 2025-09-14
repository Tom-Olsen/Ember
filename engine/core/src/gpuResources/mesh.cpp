#include "mesh.h"
#include "vulkanMesh.h"



namespace emberEngine
{
	// Constructors/Destructor:
	Mesh::Mesh(const std::string& name)
	{
		m_pIMesh = std::make_unique<vulkanRendererBackend::Mesh>(name);
	}
	Mesh::~Mesh()
	{
		m_pIMesh.release();
	}



	// Setters:
	void Mesh::SetName(const std::string& name)
	{
		m_pIMesh->SetName(name);
	}
	void Mesh::SetPositions(const std::vector<Float3>& positions)
	{
		m_pIMesh->SetPositions(positions);
	}
	void Mesh::SetNormals(const std::vector<Float3>& normals)
	{
		m_pIMesh->SetNormals(normals);
	}
	void Mesh::SetTangents(const std::vector<Float3>& tangents)
	{
		m_pIMesh->SetTangents(tangents);
	}
	void Mesh::SetColors(const std::vector<Float4>& colors)
	{
		m_pIMesh->SetColors(colors);
	}
	void Mesh::SetUniformColor(const Float4& color)
	{
		m_pIMesh->SetUniformColor(color);
	}
	void Mesh::SetUVs(const std::vector<Float4>& uvs)
	{
		m_pIMesh->SetUVs(uvs);
	}
	void Mesh::SetTriangles(const std::vector<Uint3>& triangles)
	{
		m_pIMesh->SetTriangles(triangles);
	}



	// Movers:
	void Mesh::MovePositions(std::vector<Float3>& positions)
	{
		m_pIMesh->MovePositions(positions);
	}
	void Mesh::MoveNormals(std::vector<Float3>& normals)
	{
		m_pIMesh->MoveNormals(normals);
	}
	void Mesh::MoveTangents(std::vector<Float3>& tangents)
	{
		m_pIMesh->MoveTangents(tangents);
	}
	void Mesh::MoveColors(std::vector<Float4>& colors)
	{
		m_pIMesh->MoveColors(colors);
	}
	void Mesh::MoveUVs(std::vector<Float4>& uvs)
	{
		m_pIMesh->MoveUVs(uvs);
	}
	void Mesh::MoveTriangles(std::vector<Uint3>& triangles)
	{
		m_pIMesh->MoveTriangles(triangles);
	}

	// Getters:
	const std::string& Mesh::GetName() const
	{
		return m_pIMesh->GetName();
	}
	uint32_t Mesh::GetVertexCount() const
	{
		return m_pIMesh->GetVertexCount();
	}
	uint32_t Mesh::GetTriangleCount() const
	{
		return m_pIMesh->GetTriangleCount();
	}
	std::vector<Float3>& Mesh::GetPositions()
	{
		return m_pIMesh->GetPositions();
	}
	std::vector<Float3>& Mesh::GetNormals()
	{
		return m_pIMesh->GetNormals();
	}
	std::vector<Float3>& Mesh::GetTangents()
	{
		return m_pIMesh->GetTangents();
	}
	std::vector<Float4>& Mesh::GetColors()
	{
		return m_pIMesh->GetColors();
	}
	std::vector<Float4>& Mesh::GetUVs()
	{
		return m_pIMesh->GetUVs();
	}
	std::vector<Uint3>& Mesh::GetTriangles()
	{
		return m_pIMesh->GetTriangles();
	}



	// Mesh transformations (changes *this):
	void Mesh::ComputeNormals()
	{
		m_pIMesh->ComputeNormals();
	}
	void Mesh::ComputeTangents()
	{
		m_pIMesh->ComputeTangents();
	}
}