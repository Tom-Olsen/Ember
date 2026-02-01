#pragma once
#include "commonVertexMemoryLayout.h"
#include "emberMath.h"
#include "iMesh.h"
#include "vulkanIndexType.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class IndexBuffer;
	class StagingBuffer;
	class VertexBuffer;



	/// <summary>
	/// Mesh class does not guarantee validity of the mesh data. It only provides GPU resources for rendering.
	/// </summary>
	class VULKAN_RENDERER_API Mesh : public emberBackendInterface::IMesh
	{
	public: // Enums/Structs:
		//enum class MeshType
		//{
		//	staticMesh,
		//	dynamicMesh
		//};
		struct Vertex
		{
			Float3 position;
			Float3 normal;
			Float3 tangent;
			Float4 color;
			Float4 uv;
		};

	private: // Members:
		//MeshType m_meshType = MeshType::static;	// Ember::ToDo: implement static meshes.
		VkIndexType m_vkIndexType = VK_INDEX_TYPE_UINT16;
		emberCommon::VertexMemoryLayout m_vertexMemoryLayout = emberCommon::VertexMemoryLayout::interleaved;
		std::unique_ptr<StagingBuffer> m_pVertexStagingBuffer;
		std::unique_ptr<StagingBuffer> m_pIndexStagingBuffer;
		std::vector<std::unique_ptr<VertexBuffer>> m_pVertexBuffers;
		std::vector<std::unique_ptr<IndexBuffer>> m_pIndexBuffers;
		std::vector<std::vector<VkBuffer>> m_vkBuffersCache;
		std::vector<std::vector<VkDeviceSize>> m_vkOffsetsCache;

	public: // Methods:
		// Constructors/Destructor:
		Mesh();
		~Mesh();

		// Non-copyable:
		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;

		// Movable:
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other) noexcept;

		// Setters:
		//void SetMeshType(MeshType type);

		// Getters:
		//MeshType GetMeshType() const;
		emberCommon::VertexMemoryLayout GetVertexMemoryLayout() const override;

		// Update GPU buffers:
		void UpdateVertexBuffer(const std::vector<Float3>& positions, std::vector<Float3>* pNormals, std::vector<Float3>* pTangents, std::vector<Float4>* pColors, std::vector<Float4>* pUvs, emberCommon::VertexMemoryLayout vertexMemoryLayout) override;
		void UpdateVertexBuffer(const std::vector<Float3>& positions, std::vector<Float3>* pNormals, std::vector<Float3>* pTangents, std::vector<Float4>* pColors, std::vector<Float4>* pUvs) override;
		void UpdateIndexBuffer(const std::vector<Uint3>& triangles, uint32_t vertexCount = 0) override;

		// Backend only:
		VkIndexType GetVkIndexType() const;
		VertexBuffer* GetVertexBuffer();
		VertexBuffer* GetVertexBuffer(uint32_t frameIndex);
		IndexBuffer* GetIndexBuffer();
		IndexBuffer* GetIndexBuffer(uint32_t frameIndex);
		VkBuffer* GetVkBuffers();
		VkDeviceSize* GetVkOffsets();
		void RecordUpdateCommand(VkCommandBuffer vkCommandBuffer, uint32_t frameIndex);

	private:
		void UpdateBufferCache(uint32_t frameIndex, uint32_t vertexCount):
		template<typename T>
		void WriteArrayToVertexStagingBuffer(const T* pSrc, size_t count, size_t offset, const T& defaultValue);
	};
}