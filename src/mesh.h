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
/// Mesh takes ownership of vectors.
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
	uint32_t Size() const;
	uint32_t SizeOfBuffer() const;
	uint32_t SizeOfPositions() const;
	uint32_t SizeOfColors() const;
	uint32_t SizeOfTriangles() const;

	// Static methods:
	static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	static VkIndexType GetIndexType();
};

std::string to_string(Mesh mesh);
#endif // __INCLUDE_GUARD_mesh_h__