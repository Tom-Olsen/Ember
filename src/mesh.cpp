#include "mesh.h"



// Constructor:
Mesh::Mesh()
{

}



// Destructor:
Mesh::~Mesh()
{

}



// Public:
// Setters:
void Mesh::SetPositions(std::vector<Float3>&& positions)
{
	this->vertexCount = static_cast<uint32_t>(positions.size());
	this->positions = std::move(positions);
}
void Mesh::SetColors(std::vector<Float4>&& colors)
{
	this->colors = std::move(colors);
}
void Mesh::SetTriangles(std::vector<Int3>&& triangles)
{
	this->triangleCount = static_cast<uint32_t>(triangles.size());
	this->triangles = std::move(triangles);
}

// Getters:
uint32_t Mesh::GetVertexCount() const
{
	return vertexCount;
}
uint32_t Mesh::GetTriangleCount() const
{
	return triangleCount;
}
std::vector<Float3> Mesh::GetPositions() const
{
	return positions;
}
std::vector<Float4> Mesh::GetColors() const
{
	return colors;
}
std::vector<Int3> Mesh::GetTriangles() const
{
	return triangles;
}
uint32_t* Mesh::GetTrianglesUnrolled()
{
	return reinterpret_cast<uint32_t*>(triangles.data());
}
uint32_t Mesh::Size() const
{
	return SizeOfPositions() + SizeOfColors() + SizeOfTriangles();
}
uint32_t Mesh::SizeOfBuffer() const
{
	return SizeOfPositions() + SizeOfColors();
}
uint32_t Mesh::SizeOfPositions() const
{
	return vertexCount * sizeof(Float3);
}
uint32_t Mesh::SizeOfColors() const
{
	return vertexCount * sizeof(Float4);
}
uint32_t Mesh::SizeOfTriangles() const
{
	return triangleCount * sizeof(Int3);
}



// Non-member:
std::string to_string(Mesh mesh)
{
	std::string str = "Mesh:\n";

	str += "Vertices:\n";
	for (const Float3& position : mesh.GetPositions())
		str += to_string(position) + "\n";

	str += "Colors:\n";
	for (const Float4& color : mesh.GetColors())
		str += to_string(color) + "\n";

	str += "Triangles:\n";
	for (const Int3& triangle : mesh.GetTriangles())
		str += to_string(triangle) + "\n";
	return str;
}