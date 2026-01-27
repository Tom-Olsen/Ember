#include "vulkanMesh.h"
#include "logger.h"
#include "vulkanIndexBuffer.h"
#include "vulkanContext.h"
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
	void Mesh::SetMeshType(Mesh::MeshType type)
	{
		m_meshType = type;
	}
	void Mesh::SetIndexType(Mesh::IndexType type)
	{
		m_indexType = type;
	}
	void Mesh::SetMemoryLayout(Mesh::MemoryLayout layout)
	{
		m_memoryLayout = layout;
	}



	// Getters:
	const std::string& Mesh::GetName() const
	{
		return m_name;
	}
	Mesh::MeshType Mesh::GetMeshType() const
	{
		return m_meshType;
	}
	Mesh::IndexType Mesh::GetIndexType() const
	{
		return m_indexType;
	}
	Mesh::MemoryLayout Mesh::GetMemoryLayout() const
	{
		return m_memoryLayout;
	}
	emberBackendInterface::IMesh* Mesh::GetCopy(const std::string& newName)
	{
		Mesh* copy = new Mesh(newName);
		copy->SetMeshType(m_meshType);
		copy->SetIndexType(m_indexType);
		copy->SetMemoryLayout(m_memoryLayout);
		return static_cast<emberBackendInterface::IMesh*>(copy);
	}



		// Backend only:
	VertexBuffer* Mesh::GetVertexBuffer()
	{
		if (m_meshType == MeshType::dynamic)
			return m_pVertexBuffers[0].get();
		else
			return m_pVertexBuffers[Context::GetFrameIndex()].get();
	}
	IndexBuffer* Mesh::GetIndexBuffer()
	{
		if (m_meshType == MeshType::static)
			return m_pIndexBuffers[0].get();
		else
			return m_pIndexBuffers[Context::GetFrameIndex()].get();
	}
	//uint64_t Mesh::GetPositionsOffset() const
	//{
	//	return 0;
	//}
	//uint64_t Mesh::GetNormalsOffset() const
	//{
	//	return GetSizeOfPositions();
	//}
	//uint64_t Mesh::GetTangentsOffset() const
	//{
	//	return GetSizeOfPositions() + GetSizeOfNormals();
	//}
	//uint64_t Mesh::GetColorsOffset() const
	//{
	//	return GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents();
	//}
	//uint64_t Mesh::GetUVsOffset() const
	//{
	//	return GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors();
	//}



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
	void Mesh::UpdateIndexBuffer()
	{
		static_assert(sizeof(Uint3) == 3 * sizeof(uint32_t));
		size_t triangleCount = triangles->size();

		// Updating static mesh forces wait for previous render calls to finish as mesh could be in use already:
		if (m_meshType == MeshType::static)
			vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);

		// Resize buffers if necessary:
		for (size_t i = 0; i < m_pIndexBuffers.size(); i++)
		{
			if (m_pIndexBuffers[i] == nullptr || m_pIndexBuffers[i]->GetCount() != triangleCount)
				m_pIndexBuffers[i] = std::make_unique<IndexBuffer>(triangleCount, 3 * (uint32_t)sizeof(uint32_t), m_name);
		}

		// Copy: triangles -> stagingBuffer -> indexBuffer
		uint64_t size = triangleCount * sizeof(Uint3);
		StagingBuffer stagingBuffer(size, m_name);
		stagingBuffer.SetData(reinterpret_cast<uint32_t*>(m_triangles.data()), size);
		stagingBuffer.UploadToBuffer(m_pIndexBuffer.get(), Context::GetLogicalDevice()->GetGraphicsQueue());
	}
	#endif
}