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



// Static methods:
std::vector<VkVertexInputBindingDescription> Mesh::GetBindingDescription()
{
	// binding = index of the binding in the array of bindings
	// stride = number of bytes from one entry to the next
	// inputRate = VK_VERTEX_INPUT_RATE_VERTEX : move to the next data entry after each vertex
	//             VK_VERTEX_INPUT_RATE_INSTANCE : move to the next data entry after each instance
	// => x0y0z0, x1y1z1, ..., r0g0b0a0, r1g1b1a1, ...
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;

	// position binding:
	VkVertexInputBindingDescription positionBindingDescription = {};
	positionBindingDescription.binding = 0;
	positionBindingDescription.stride = sizeof(Float3);
	positionBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescriptions.push_back(positionBindingDescription);

	// color binding:
	VkVertexInputBindingDescription colorBindingDescription = {};
	colorBindingDescription.binding = 1;
	colorBindingDescription.stride = sizeof(Float4);
	colorBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescriptions.push_back(colorBindingDescription);

	return bindingDescriptions;
}
std::vector<VkVertexInputAttributeDescription> Mesh::GetAttributeDescriptions()
{
	// binding = index of the attribute in the array of attributes
	// location = reference to the location directive of the input in the vertex shader
	// format = how to interpret the data
	// offset = data offset
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	// position attribute:
	VkVertexInputAttributeDescription positionAttributeDescription = {};
	positionAttributeDescription.binding = 0;
	positionAttributeDescription.location = 0;
	positionAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttributeDescription.offset = 0;
	attributeDescriptions.push_back(positionAttributeDescription);

	// color attribute:
	VkVertexInputAttributeDescription colorAttributeDescription = {};
	colorAttributeDescription.binding = 1;
	colorAttributeDescription.location = 1;
	colorAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	colorAttributeDescription.offset = 0;
	attributeDescriptions.push_back(colorAttributeDescription);

	return attributeDescriptions;
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