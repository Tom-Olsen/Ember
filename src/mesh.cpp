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
void Mesh::SetUVs(std::vector<Float4>&& uvs)
{
	this->uvs = std::move(uvs);
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
std::vector<Float3>& Mesh::GetPositions()
{
	return positions;
}
std::vector<Float4>& Mesh::GetColors()
{
	return colors;
}
std::vector<Float4>& Mesh::GetUVs()
{
	return uvs;
}
std::vector<Int3>& Mesh::GetTriangles()
{
	return triangles;
}
uint32_t* Mesh::GetTrianglesUnrolled()
{
	return reinterpret_cast<uint32_t*>(triangles.data());
}
uint32_t Mesh::GetSize() const
{
	return GetSizeOfPositions() + GetSizeOfColors() + GetSizeOfTriangles();
}
uint32_t Mesh::GetSizeOfBuffer() const
{
	return GetSizeOfPositions() + GetSizeOfColors() + GetSizeOfUVs();
}
uint32_t Mesh::GetSizeOfPositions() const
{
	return vertexCount * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfColors() const
{
	return vertexCount * sizeof(Float4);
}
uint32_t Mesh::GetSizeOfUVs() const
{
	return vertexCount * sizeof(Float4);
}
uint32_t Mesh::GetSizeOfTriangles() const
{
	return triangleCount * sizeof(Int3);
}
std::vector<uint64_t> Mesh::GetBufferSizes() const
{
	return std::vector<uint64_t>({ GetSizeOfPositions(), GetSizeOfColors(), GetSizeOfUVs() });
}
std::vector<void*> Mesh::GetBufferDatas()
{
	return std::vector<void*>({ static_cast<void*>(positions.data()), static_cast<void*>(colors.data()), static_cast<void*>(uvs.data()) });
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

	// uv binding:
	VkVertexInputBindingDescription uvBindingDescription = {};
	uvBindingDescription.binding = 2;
	uvBindingDescription.stride = sizeof(Float4);
	uvBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescriptions.push_back(uvBindingDescription);

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

	// uv attribute:
	VkVertexInputAttributeDescription uvAttributeDescription = {};
	uvAttributeDescription.binding = 2;
	uvAttributeDescription.location = 2;
	uvAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	uvAttributeDescription.offset = 0;
	attributeDescriptions.push_back(uvAttributeDescription);

	return attributeDescriptions;
}
VkIndexType Mesh::GetIndexType()
{
	return VK_INDEX_TYPE_UINT32;
}
uint32_t Mesh::GetBindingCount()
{
	return 3;
}
std::vector<VkDeviceSize> Mesh::GetOffsets()
{
	// Edit this when adding more buffers to the mesh.
	// offsets = { position offset, color offset, uv offset, ... }
	std::vector<VkDeviceSize> offsets = { 0, GetSizeOfPositions(), GetSizeOfPositions() + GetSizeOfColors() };
	return offsets;
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