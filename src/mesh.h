#ifndef __INCLUDE_GUARD_mesh_h__
#define __INCLUDE_GUARD_mesh_h__
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "glmTypes.h"
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanBuffer.h"



// TODO:
// - static and dynamic meshes?
// - move vertex/index buffers to mesh?



/// <summary>
/// Mesh class for storing vertex positions, colors and triangles.
/// Mesh class takes ownership of vectors and takes care of cleanup.
/// </summary>
class Mesh
{
private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	bool verticesUpdated = false;
	bool indicesUpdated = false;
	uint32_t vertexCount = 0;
	uint32_t triangleCount = 0;
	std::unique_ptr<VulkanBuffer> vertexBuffer;
	std::unique_ptr<VulkanBuffer> indexBuffer;
	std::vector<Float3> positions;
	std::vector<Float4> colors;
	std::vector<Float4> uvs;
	std::vector<Int3> triangles;

public: // Methods:
	Mesh(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice);
	Mesh(const Mesh& other) = default;
	Mesh& operator=(const Mesh& other) = default;
	Mesh(Mesh&& other) noexcept = default;
	Mesh& operator=(Mesh&& other) noexcept = default;
	~Mesh();
	
	// Setters:
	void SetPositions(std::vector<Float3>&& positions);
	void SetColors(std::vector<Float4>&& colors);
	void SetUVs(std::vector<Float4>&& uvs);
	void SetTriangles(std::vector<Int3>&& triangles);

	// Getters:
	uint32_t GetVertexCount() const;
	uint32_t GetTriangleCount() const;
	std::vector<Float3>& GetPositions();
	std::vector<Float4>& GetColors();
	std::vector<Float4>& GetUVs();
	std::vector<Int3>& GetTriangles();
	uint32_t* GetTrianglesUnrolled();
	uint32_t GetSize() const;
	uint32_t GetSizeOfBuffer() const;
	uint32_t GetSizeOfPositions() const;
	uint32_t GetSizeOfColors() const;
	uint32_t GetSizeOfUVs() const;
	uint32_t GetSizeOfTriangles() const;
	std::vector<uint64_t> GetBufferSizes() const;
	std::vector<void*> GetBufferDatas();
	std::vector<VkDeviceSize> GetOffsets();
	VulkanBuffer* GetVertexBuffer();
	VulkanBuffer* GetIndexBuffer();

	// Static methods:
	static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	static VkIndexType GetIndexType();
	static uint32_t GetBindingCount();

private: // Methods:
	void UpdateVertexBuffer();
	void UpdateIndexBuffer();
};

// Methods that must be defined outside of the class:
std::string to_string(Mesh mesh);



#endif // __INCLUDE_GUARD_mesh_h__