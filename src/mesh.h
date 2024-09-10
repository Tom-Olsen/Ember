#ifndef __INCLUDE_GUARD_mesh_h__
#define __INCLUDE_GUARD_mesh_h__
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "glmTypes.h"



// TODO:
// - static and dynamic meshes?
// - move vertex/index buffers to mesh?
// - combine vertex and index data into a single buffer and use offsets.



/// <summary>
/// Mesh class for storing vertex positions, colors and triangles.
/// Mesh class takes ownership of vectors and takes care of cleanup.
/// </summary>
class Mesh
{
private: // Members:
	uint32_t vertexCount = 0;
	uint32_t triangleCount = 0;
	std::vector<Float3> positions;
	std::vector<Float4> colors;
	std::vector<Int3> triangles;


public: // Methods:
	Mesh();
	Mesh(const Mesh& other) = default;
	Mesh& operator=(const Mesh& other) = default;
	Mesh(Mesh&& other) noexcept = default;
	Mesh& operator=(Mesh&& other) noexcept = default;
	~Mesh();
	
	// Setters:
	void SetPositions(std::vector<Float3>&& positions);
	void SetColors(std::vector<Float4>&& colors);
	void SetTriangles(std::vector<Int3>&& triangles);

	// Getters:
	uint32_t GetVertexCount() const;
	uint32_t GetTriangleCount() const;
	std::vector<Float3> GetPositions() const;
	std::vector<Float4> GetColors() const;
	std::vector<Int3> GetTriangles() const;
	uint32_t* GetTrianglesUnrolled();
	uint32_t GetSize() const;
	uint32_t GetSizeOfBuffer() const;
	uint32_t GetSizeOfPositions() const;
	uint32_t GetSizeOfColors() const;
	uint32_t GetSizeOfTriangles() const;

	// Static methods:
	static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	static VkIndexType GetIndexType();
};

// Methods that must be defined outside of the class:
std::string to_string(Mesh mesh);



#endif // __INCLUDE_GUARD_mesh_h__