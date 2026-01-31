#include "vulkanMesh.h"
#include "logger.h"
#include "vulkanIndexBuffer.h"
#include "vulkanContext.h"
#include "vulkanGarbageCollector.h"
#include "vulkanLogicalDevice.h"
#include "vulkanStagingBuffer.h"
#include "vulkanVertexBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Mesh::Mesh(const std::string& name)
	{
		m_name = name;
		m_pVertexBuffers.resize(Context::GetFramesInFlight());
		m_pIndexBuffers.resize(Context::GetFramesInFlight());
	}
	Mesh::~Mesh()
	{

	}



	// Movable:
	Mesh::Mesh(Mesh&& other) noexcept = default;
	Mesh& Mesh::operator=(Mesh&& other) noexcept = default;



	// Setters:
	void Mesh::SetName(const std::string& name)
	{
		m_name = name;
	}
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
	void Mesh::SetMemoryLayout(Mesh::MemoryLayout layout)
	{
		m_memoryLayout = layout;
	}



	// Getters:
	const std::string& Mesh::GetName() const
	{
		return m_name;
	}
	//Mesh::MeshType Mesh::GetMeshType() const
	//{
	//	return m_meshType;
	//}
	Mesh::VkIndexType Mesh::GetVkIndexType() const
	{
		return m_vkIndexType;
	}
	//Mesh::MemoryLayout Mesh::GetMemoryLayout() const
	//{
	//	return m_memoryLayout;
	//}
	emberBackendInterface::IMesh* Mesh::GetCopy(const std::string& newName)
	{
		Mesh* copy = new Mesh(newName);
		//copy->SetMeshType(m_meshType);
		//copy->SetMemoryLayout(m_memoryLayout);
		return static_cast<emberBackendInterface::IMesh*>(copy);
	}



	// Backend only:
	VertexBuffer* Mesh::GetVertexBuffer()
	{
		return m_pVertexBuffers[Context::GetFrameIndex()]->GetActiveBuffer();
	}
	IndexBuffer* Mesh::GetIndexBuffer()
	{
		return m_pIndexBuffers[Context::GetFrameIndex()]->GetActiveBuffer();
	}



	// Update GPU buffers:
	#ifdef RESIZEABLE_BAR // No staging buffer: not implemented yet
	//void Mesh::UpdateVertexBuffer(std::vector<Float3>* positions, std::vector<Float3>* normals, std::vector<Float3>* tangents, std::vector<Float4>* colors, std::vector<Float4>* uvs)
	//{
	//	size_t vertexCount = positions->size();
	//
	//	// Updating static mesh forces wait for previous render calls to finish as mesh could be in use already:
	//	if (m_meshType == MeshType::static)
	//		vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);
	//
	//	// Resize buffer if necessary:
	//	for (size_t i = 0; i < m_pVertexBuffers.size(); i++)
	//	{
	//		if (m_pVertexBuffers[i] == nullptr || m_pVertexBuffers[i]->GetCount() != vertexCount)
	//			m_pVertexBuffers[i] = std::make_unique<VertexBuffer>(vertexCount, sizeof(Vertex), m_name);
	//	}
	//
	//	// Copy: meshData -> vertexBuffer
	//	if (m_memoryLayout == MemoryLayout::interleaved)
	//	{
	//		for (size_t i = 0; i < m_pVertexBuffers.size(); i++)
	//		{
	//			void* data;
	//			VKA(vmaMapMemory(Context::GetVmaAllocator(), m_pVertexBuffers[i]->allocation, &data));
	//			for (size_t j = 0; j < vertexCount; j++)
	//			{
	//				Vertex vertex;
	//				vertex.position = (*positions)[j];
	//				vertex.normal = normals ? (*normals)[j] : Float3::up;
	//				vertex.tangent = tangents ? (*tangents)[j] : Float3::right;
	//				vertex.color = colors ? (*colors)[j] : Float4::white;
	//				vertex.uv = uvs ? (*uvs)[j] : Float4::zero;
	//				memcpy(static_cast<char*>(data) + j * sizeof(Vertex), &vertex, sizeof(Vertex));
	//			}
	//			VKA(vmaUnmapMemory(Context::GetVmaAllocator(), m_pVertexBuffers[i]->allocation));
	//		}
	//	}
	//	else // (m_memoryLayout == MemoryLayout::separate)
	//	{
	//		size_t posEnd = vertexCount * sizeof(Float3);
	//		size_t normEnd = posEnd + vertexCount * sizeof(Float3);
	//		size_t tangEnd = normEnd + vertexCount * sizeof(Float3);
	//		size_t colorEnd = tangEnd + vertexCount * sizeof(Float4);
	//		size_t uvEnd = colorEnd + vertexCount * sizeof(Float4);
	//		for (size_t i = 0; i < m_pVertexBuffers.size(); i++)
	//		{
	//			void* data;
	//			VKA(vmaMapMemory(Context::GetVmaAllocator(), m_pVertexBuffers[i]->allocation, &data));
	//			if (positions) memcpy(static_cast<char*>(data), positions->data(), posEnd);
	//			if (normals) memcpy(static_cast<char*>(data) + posEnd, normals->data(), normEnd);
	//			if (tangents) memcpy(static_cast<char*>(data) + normEnd, tangents->data(), tangEnd);
	//			if (colors) memcpy(static_cast<char*>(data) + tangEnd, colors->data(), colorEnd);
	//			if (uvs) memcpy(static_cast<char*>(data) + colorEnd, uvs->data(), uvEnd);
	//			VKA(vmaUnmapMemory(Context::GetVmaAllocator(), m_pVertexBuffers[i]->allocation));
	//		}
	//	}
	//}
	#else // With Staging buffer:
	void Mesh::UpdateVertexBuffer(std::vector<Float3>* positions, std::vector<Float3>* normals, std::vector<Float3>* tangents, std::vector<Float4>* colors, std::vector<Float4>* uvs)
	{
		size_t vertexCount = positions->size();
		m_vkIndexType = (vertexCount > 65535) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16; 

		// Updating static mesh forces wait for previous render calls to finish as mesh could be in use already:
		if (m_meshType == MeshType::static)
			vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);

		// Resize buffers if necessary:
		for (size_t i = 0; i < m_pVertexBuffers.size(); i++)
		{
			if (m_pVertexBuffers[i] == nullptr || m_pVertexBuffers[i]->GetCount() != vertexCount)
				m_pVertexBuffers[i] = std::make_unique<VertexBuffer>(vertexCount, sizeof(Vertex), m_name);
		}

		// Copy: meshData -> stagingBuffer -> vertexBuffer
		if (m_memoryLayout == MemoryLayout::interleaved)
		{
			for (size_t i = 0; i < m_pVertexBuffers.size(); i++)
			{
				StagingBuffer stagingBuffer(vertexCount * sizeof(Vertex), m_name);
				for (size_t j = 0; j < vertexCount; j++)
				{
					Vertex vertex;
					vertex.position = (*positions)[j];
					vertex.normal = normals ? (*normals)[j] : Float3::up;
					vertex.tangent = tangents ? (*tangents)[j] : Float3::right;
					vertex.color = colors ? (*colors)[j] : Float4::white;
					vertex.uv = uvs ? (*uvs)[j] : Float4::zero;
					stagingBuffer.SetData(&vertex, sizeof(Vertex), j * sizeof(Vertex));
				}
				stagingBuffer.UploadToBuffer(m_pVertexBuffers[i].get(), Context::GetLogicalDevice()->GetGraphicsQueue());
			}
		}
		else
		{
			for (size_t i = 0; i < m_pVertexBuffers.size(); i++)
			{
				size_t posEnd = vertexCount * sizeof(Float3);
				size_t normEnd = posEnd + vertexCount * sizeof(Float3);
				size_t tangEnd = normEnd + vertexCount * sizeof(Float3);
				size_t colorEnd = tangEnd + vertexCount * sizeof(Float4);
				size_t uvEnd = colorEnd + vertexCount * sizeof(Float4);
				StagingBuffer stagingBuffer(vertexCount * sizeof(Vertex), m_name);
				stagingBuffer.SetData(positions->data(), 0, posEnd);
				stagingBuffer.SetData(normals->data(), posEnd, normEnd);
				stagingBuffer.SetData(tangents->data(), normEnd, tangEnd);
				stagingBuffer.SetData(colors->data(), tangEnd, colorEnd);
				stagingBuffer.SetData(uvs->data(), colorEnd, uvEnd);
				stagingBuffer.UploadToBuffer(m_pVertexBuffers[i].get(), Context::GetLogicalDevice()->GetGraphicsQueue());
			}
		}
	}
	#endif
	#ifdef RESIZEABLE_BAR // No staging buffer: not implemented yet
	//void Mesh::UpdateIndexBuffer(std::vector<Uint3>* triangles)
	//{
	//	static_assert(sizeof(Uint3) == 3 * sizeof(uint32_t));
	//	size_t triangleCount = triangles->size();
	//
	//	// Updating static mesh forces wait for previous render calls to finish as mesh could be in use already:
	//	if (m_meshType == MeshType::static)
	//		vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);
	//
	//	// Resize buffer if necessary:
	//	for (size_t i = 0; i < m_pIndexBuffers.size(); i++)
	//	{
	//		if (m_pIndexBuffers[i] == nullptr || m_pIndexBuffers[i]->GetCount() != triangleCount)
	//			m_pIndexBuffers[i] = std::make_unique<IndexBuffer>(triangleCount, 3 * (uint32_t)sizeof(uint32_t), m_name);
	//	}
	//
	//	// Copy: triangles -> indexBuffer
	//	uint64_t size = triangleCount * sizeof(Uint3);
	//	void* data;
	//	VKA(vmaMapMemory(Context::GetVmaAllocator(), m_pIndexBuffer->allocation, &data));
	//	memcpy(data, reinterpret_cast<uint32_t*>(m_triangles.data()), size);
	//	VKA(vmaUnmapMemory(Context::GetVmaAllocator(), m_pIndexBuffer->allocation));
	//}
	#else // With Staging buffer:
	void Mesh::UpdateIndexBuffer(std::vector<Uint3>* triangles)
	{
		static_assert(sizeof(Uint3) == 3 * sizeof(uint32_t));
		size_t triangleCount = triangles->size();

		if (m_meshType == MeshType::static)
		{
			// Updating static mesh forces wait for previous render calls to finish as mesh could be in use already:
			vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);

			// Resize buffers if necessary:
			uint32_t elemetSize = (m_vkIndexType == VK_INDEX_TYPE_UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);	
			if (m_pIndexBuffers[0] == nullptr || m_pIndexBuffers[0]->GetCount() != triangleCount || m_pIndexBuffers[0]->GetElementSize() != elemetSize)
				m_pIndexBuffers[0] = std::make_unique<IndexBuffer>(triangleCount, 3 * elemetSize, m_name);

			if (m_vkIndexType == VK_INDEX_TYPE_UINT16)
			{
				// Convert triangles to uint16_t:
				std::vector<uint16_t> triangles16(3 * triangleCount);
				for (size_t i = 0; i < triangleCount; i++)
				{
					triangles16[0 + 3 * i] = static_cast<uint16_t>((*triangles)[i].x);
					triangles16[1 + 3 * i] = static_cast<uint16_t>((*triangles)[i].y);
					triangles16[2 + 3 * i] = static_cast<uint16_t>((*triangles)[i].z);
				}

				// Copy: triangles(16bit) -> stagingBuffer -> indexBuffer
				uint64_t size = 3 * triangleCount * sizeof(uint16_t);
				StagingBuffer stagingBuffer(size, m_name);
				stagingBuffer.SetData(reinterpret_cast<uint16_t*>(triangles16.data()), size);
				stagingBuffer.UploadToBuffer(m_pIndexBuffers[0].get(), Context::GetLogicalDevice()->GetGraphicsQueue());
			}
			else // (m_indexType == IndexType::uint32)
			{
				// Copy: triangles(32bit) -> stagingBuffer -> indexBuffer
				uint64_t size = 3 * triangleCount * sizeof(uint32_t);
				StagingBuffer stagingBuffer(size, m_name);
				stagingBuffer.SetData(reinterpret_cast<uint32_t*>(triangles->data()), size);
				stagingBuffer.UploadToBuffer(m_pIndexBuffers[0].get(), Context::GetLogicalDevice()->GetGraphicsQueue());
			}
		}
		else // (m_meshType == MeshType::dynamic)
		{

		}



		// Resize buffers if necessary:
		for (size_t i = 0; i < m_pIndexBuffers.size(); i++)
		{
			if (m_pIndexBuffers[i] == nullptr || m_pIndexBuffers[i]->GetCount() != triangleCount)
				m_pIndexBuffers[i] = std::make_unique<IndexBuffer>(triangleCount, 3 * (uint32_t)sizeof(uint32_t), m_name);
		}

		if (m_vkIndexType == VK_INDEX_TYPE_UINT16)
		{
			// Convert triangles to uint16_t:
			std::vector<uint16_t> triangles16(3 * triangleCount);
			for (size_t i = 0; i < triangleCount; i++)
			{
				triangles16[0 + 3 * i] = static_cast<uint16_t>((*triangles)[i].x);
				triangles16[1 + 3 * i] = static_cast<uint16_t>((*triangles)[i].y);
				triangles16[2 + 3 * i] = static_cast<uint16_t>((*triangles)[i].z);
			}

			// Copy: triangles(16bit) -> stagingBuffer -> indexBuffer
			uint64_t size = 3 * triangleCount * sizeof(uint16_t);
			StagingBuffer stagingBuffer(size, m_name);
			stagingBuffer.SetData(reinterpret_cast<uint16_t*>(triangles16.data()), size);
			stagingBuffer.UploadToBuffer(m_pIndexBuffers[0].get(), Context::GetLogicalDevice()->GetGraphicsQueue());
		}
		else // (m_indexType == IndexType::uint32)
		{
			// Copy: triangles(32bit) -> stagingBuffer -> indexBuffer
			uint64_t size = 3 * triangleCount * sizeof(uint32_t);
			StagingBuffer stagingBuffer(size, m_name);
			stagingBuffer.SetData(reinterpret_cast<uint32_t*>(triangles->data()), size);
			stagingBuffer.UploadToBuffer(m_pIndexBuffers[0].get(), Context::GetLogicalDevice()->GetGraphicsQueue());
		}
	}
	#endif
}