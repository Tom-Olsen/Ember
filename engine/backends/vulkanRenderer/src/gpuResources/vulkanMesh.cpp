#include "vulkanMesh.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vulkanIndexBuffer.h"
#include "vulkanContext.h"
#include "vulkanGarbageCollector.h"
#include "vulkanLogicalDevice.h"
#include "vulkanRenderer.h"
#include "vulkanStagingBuffer.h"
#include "vulkanVertexBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Mesh::Mesh()
	{
		size_t frames = Context::GetFramesInFlight();
		m_pVertexBuffers.resize(frames);
		m_pIndexBuffers.resize(frames);
		m_buffersCache.resize(frames);
		m_offsetsCache.resize(frames);
	}
	Mesh::~Mesh()
	{

	}



	// Movable:
	Mesh::Mesh(Mesh&& other) noexcept = default;
	Mesh& Mesh::operator=(Mesh&& other) noexcept = default;



	// Setters:
	//void Mesh::SetMeshType(Mesh::MeshType type)
	//{
	//	if (m_meshType != type)
	//	{
	//		m_meshType = type;
	//		size_t bufferCount = (type == MeshType::static) ? 1 : Context::GetFramesInFlight();
	//		m_pVertexBuffers.resize(bufferCount);
	//		m_pIndexBuffers.resize(bufferCount);
	//	}
	//}



	// Getters:
	//Mesh::MeshType Mesh::GetMeshType() const
	//{
	//	return m_meshType;
	//}
	emberCommon::VertexMemoryLayout Mesh::GetVertexMemoryLayout() const
	{
		return m_vertexMemoryLayout;
	}



	// Update GPU buffers:
	void Mesh::UpdateVertexBuffer(const std::vector<Float3>& positions, std::vector<Float3>* pNormals, std::vector<Float3>* pTangents, std::vector<Float4>* pColors, std::vector<Float4>* pUvs, emberCommon::VertexMemoryLayout vertexMemoryLayout)
	{
		m_vertexMemoryLayout = vertexMemoryLayout;
		UpdateVertexBuffer(positions, pNormals, pTangents, pColors, pUvs);
	}
	void Mesh::UpdateVertexBuffer(const std::vector<Float3>& positions, std::vector<Float3>* pNormals, std::vector<Float3>* pTangents, std::vector<Float4>* pColors, std::vector<Float4>* pUvs)
	{
		size_t vertexCount = positions.size();

		// Reset staging buffer:
		m_pVertexStagingBuffer.reset();
		m_pVertexStagingBuffer = std::make_unique<StagingBuffer>(vertexCount * sizeof(Vertex));

		// Copy: meshData -> stagingBuffer
		if (m_vertexMemoryLayout == emberCommon::VertexMemoryLayout::interleaved)
		{
			for (size_t j = 0; j < vertexCount; j++)
			{
				Vertex vertex;
				vertex.position	= positions[j];
				vertex.normal	= pNormals	? (*pNormals)[j]	: Float3::up;
				vertex.tangent	= pTangents	? (*pTangents)[j]	: Float3::right;
				vertex.color	= pColors	? (*pColors)[j]		: Float4::white;
				vertex.uv		= pUvs		? (*pUvs)[j]		: Float4::zero;
				m_pVertexStagingBuffer->SetData(&vertex, sizeof(Vertex), j * sizeof(Vertex));
			}
		}
		else
		{
			const size_t positionsOffset = 0;
			const size_t normalsOffset	= positionsOffset + vertexCount * sizeof(Float3);
			const size_t tangentsOffset = normalsOffset + vertexCount * sizeof(Float3);
			const size_t colorsOffset = tangentsOffset + vertexCount * sizeof(Float3);
			const size_t uvsOffset = colorsOffset + vertexCount * sizeof(Float4);

			m_pVertexStagingBuffer->SetData(positions.data(), vertexCount * sizeof(Float3), positionsOffset);
			WriteArrayToVertexStagingBuffer(pNormals ? pNormals->data() : nullptr, vertexCount, normalsOffset, Float3::up);
			WriteArrayToVertexStagingBuffer(pTangents ? pTangents->data() : nullptr, vertexCount, tangentsOffset, Float3::right);
			WriteArrayToVertexStagingBuffer(pColors ? pColors->data() : nullptr, vertexCount, colorsOffset, Float4::white);
			WriteArrayToVertexStagingBuffer(pUvs ? pUvs->data() : nullptr, vertexCount, uvsOffset, Float4::zero);
		}

		Context::GetRenderer()->QueueMeshForUpdate(this);
	}
	void Mesh::UpdateIndexBuffer(const std::vector<Uint3>& triangles, uint32_t vertexCount)
	{
		static_assert(sizeof(Uint3) == 3 * sizeof(uint32_t));
		size_t triangleCount = triangles.size();

		// Determine index type:
		if (vertexCount > 0)
			m_vkIndexType = (vertexCount > 65535) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
		else
		{
			uint32_t maxIndex = 0;
			for (const auto& tri : triangles)
				maxIndex = std::max({ maxIndex, tri.x, tri.y, tri.z });
			m_vkIndexType = (maxIndex > 65535) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
		}
		uint32_t elemetSize = (m_vkIndexType == VK_INDEX_TYPE_UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);

		// Reset staging buffer:
		m_pIndexStagingBuffer.reset();
		m_pIndexStagingBuffer = std::make_unique<StagingBuffer>(3 * triangleCount * elemetSize);

		// Copy: triangleData -> stagingBuffer
		{
			if (m_vkIndexType == VK_INDEX_TYPE_UINT16)
			{
				// Convert triangles to uint16_t:
				std::vector<uint16_t> triangles16(3 * triangleCount);
				for (size_t i = 0; i < triangleCount; i++)
				{
					triangles16[0 + 3 * i] = static_cast<uint16_t>(triangles[i].x);
					triangles16[1 + 3 * i] = static_cast<uint16_t>(triangles[i].y);
					triangles16[2 + 3 * i] = static_cast<uint16_t>(triangles[i].z);
				}

				// Copy:
				uint64_t size = 3 * triangleCount * sizeof(uint16_t);
				m_pIndexStagingBuffer->SetData(reinterpret_cast<uint16_t*>(triangles16.data()), size);
			}
			else // (m_indexType == IndexType::uint32)
			{
				uint64_t size = 3 * triangleCount * sizeof(uint32_t);
				m_pIndexStagingBuffer->SetData(reinterpret_cast<uint32_t*>(triangles.data()), size);
			}
		}
	}



	// Backend only:
	uint32_t Mesh::GetVertexCount() const
	{
		return m_pVertexBuffers[Context::GetFrameIndex()]->GetCount();
	}
	uint32_t Mesh::GetIndexCount() const
	{
		return m_pIndexBuffers[Context::GetFrameIndex()]->GetCount();
	}
	VkIndexType Mesh::GetVkIndexType() const
	{
		return m_vkIndexType;
	}
	VertexBuffer* Mesh::GetVertexBuffer() const
	{
		return m_pVertexBuffers[Context::GetFrameIndex()].get();
	}
	VertexBuffer* Mesh::GetVertexBuffer(uint32_t frameIndex) const
	{
		return m_pVertexBuffers[frameIndex].get();
	}
	IndexBuffer* Mesh::GetIndexBuffer() const
	{
		return m_pIndexBuffers[Context::GetFrameIndex()].get();
	}
	IndexBuffer* Mesh::GetIndexBuffer(uint32_t frameIndex) const
	{
		return m_pIndexBuffers[frameIndex].get();
	}
	VkBuffer* Mesh::GetVkBuffers() const
	{
		return m_buffersCache[Context::GetFrameIndex()].data();
	}
	VkDeviceSize* Mesh::GetOffsets() const
	{
		return m_offsetsCache[Context::GetFrameIndex()].data();
	}
	uint32_t Mesh::GetVertexBindingCount() const
	{
		return m_vertexMemoryLayout == emberCommon::VertexMemoryLayout::interleaved ? 1 : 5;
	}



	void Mesh::RecordUpdateCommand(VkCommandBuffer vkCommandBuffer, uint32_t frameIndex)
	{
		uint32_t vertexCount = m_pVertexStagingBuffer->GetCount();
		uint32_t indexCount = m_pIndexStagingBuffer->GetCount();
		uint32_t elemetSize = (m_vkIndexType == VK_INDEX_TYPE_UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);

		// Resize buffers:
		bool reallocationTriggered = false;
		if (m_pVertexBuffers[frameIndex] == nullptr || m_pVertexBuffers[frameIndex]->GetCount() != vertexCount)
		{
			m_pVertexBuffers[frameIndex] = std::make_unique<VertexBuffer>(vertexCount, sizeof(Vertex));
			reallocationTriggered = true;
		}
		if (m_pIndexBuffers[frameIndex] == nullptr || m_pIndexBuffers[frameIndex]->GetCount() != indexCount || m_pIndexBuffers[frameIndex]->GetElementSize() != elemetSize)
		{
			m_pIndexBuffers[frameIndex] = std::make_unique<IndexBuffer>(indexCount, elemetSize);
			reallocationTriggered = true;
		}
		if (reallocationTriggered)
			UpdateBufferCache(frameInde1x, vertexCount);

		m_pVertexStagingBuffer->UploadToBuffer(vkCommandBuffer, m_pVertexBuffers[frameIndex]->GetVmaBuffer()->GetVkBuffer());
		m_pIndexStagingBuffer->UploadToBuffer(vkCommandBuffer, m_pIndexBuffers[frameIndex]->GetVmaBuffer()->GetVkBuffer());
	}



	// Private methods:
	void Mesh::UpdateBufferCache(uint32_t frameIndex, uint32_t vertexCount)
	{
		std::vector<VkBuffer>& buffers = m_buffersCache[frameIndex];
		std::vector<VkDeviceSize>& offsets = m_offsetsCache[frameIndex];
		buffers.clear();
		offsets.clear();
		VkBuffer vkBuffer = m_pVertexBuffers[frameIndex]->GetVmaBuffer()->GetVkBuffer();

		if (m_vertexMemoryLayout == emberCommon::VertexMemoryLayout::interleaved)
		{
			buffers.push_back(vkBuffer);
			offsets.push_back(0);
		}
		else
		{
			VkDeviceSize offset = 0;
			buffers.push_back(vkBuffer); offsets.push_back(offset);
			offset += vertexCount * sizeof(Float3);
			buffers.push_back(vkBuffer); offsets.push_back(offset);
			offset += vertexCount * sizeof(Float3);
			buffers.push_back(vkBuffer); offsets.push_back(offset);
			offset += vertexCount * sizeof(Float3);
			buffers.push_back(vkBuffer); offsets.push_back(offset);
			offset += vertexCount * sizeof(Float4);
			buffers.push_back(vkBuffer); offsets.push_back(offset);
			//offset += vertexCount * sizeof(Float4); // for future data.
		}
	}
	template<typename T>
	void Mesh::WriteArrayToVertexStagingBuffer(const T* pSrc, size_t count, size_t offset, const T& defaultValue)
	{
		const size_t sizeInBytes = count * sizeof(T);
		if (pSrc)
			m_pVertexStagingBuffer->SetData(pSrc, sizeInBytes, offset);
		else
		{
			std::vector<T> defaults(count, defaultValue);
			m_pVertexStagingBuffer->SetData(defaults.data(), sizeInBytes, offset);
		}
	}
}