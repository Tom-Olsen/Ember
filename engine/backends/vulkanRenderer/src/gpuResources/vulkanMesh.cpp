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



	// Setters: (copy the vector)
	void Mesh::SetName(const std::string& name)
	{
		m_name = name;
	}
	void Mesh::SetPositions(const std::vector<Float3>& positions)
	{
		// Copy values into member vector:
		m_vertexCount = static_cast<uint32_t>(positions.size());
		m_positions.assign(positions.begin(), positions.end());
		m_verticesUpdated = true;
	}
	void Mesh::SetNormals(const std::vector<Float3>& normals)
	{
		// Copy values into member vector and fill with default values if needed:
		m_normals.assign(normals.begin(), normals.end());
		if (normals.size() != m_vertexCount)
			m_normals.resize(m_vertexCount, Float3::up);
		m_verticesUpdated = true;
	}
	void Mesh::SetTangents(const std::vector<Float3>& tangents)
	{
		// Copy values into member vector and fill with default values if needed:
		m_tangents.assign(tangents.begin(), tangents.end());
		if (tangents.size() != m_vertexCount)
			m_tangents.resize(m_vertexCount, Float3::right);
		m_verticesUpdated = true;
	}
	void Mesh::SetColors(const std::vector<Float4>& colors)
	{
		// Copy values into member vector and fill with default values if needed:
		m_colors.assign(colors.begin(), colors.end());
		if (colors.size() != m_vertexCount)
			m_colors.resize(m_vertexCount, Float4::white);
		m_verticesUpdated = true;
	}
	void Mesh::SetUniformColor(const Float4& color)
	{
		// Set all colors to color and fill with color if needed:
		for (Float4& col : m_colors)
			col = color;
		if (m_colors.size() != m_vertexCount)
			m_colors.resize(m_vertexCount, color);
		m_verticesUpdated = true;
	}
	void Mesh::SetUVs(const std::vector<Float4>& uvs)
	{
		// Copy values into member vector and fill with default values if needed:
		m_uvs.assign(uvs.begin(), uvs.end());
		if (uvs.size() != m_vertexCount)
			m_uvs.resize(m_vertexCount, Float4::zero);
		m_verticesUpdated = true;
	}
	void Mesh::SetTriangles(const std::vector<Uint3>& triangles)
	{
		// Copy values into member vector:
		m_triangleCount = static_cast<uint32_t>(triangles.size());
		m_triangles.assign(triangles.begin(), triangles.end());
		m_indicesUpdated = true;
	}



	// Movers: (take ownership of vector)
	void Mesh::MovePositions(std::vector<Float3>&& positions)
	{
		// Take ownership if input data:
		if (&positions != &m_positions)
		{
			m_positions = std::move(positions);
			m_vertexCount = static_cast<uint32_t>(m_positions.size());
			m_verticesUpdated = true;
		}
	}
	void Mesh::MoveNormals(std::vector<Float3>&& normals)
	{
		// Take ownership of input data:
		if (&normals != &m_normals)
		{
			m_normals = std::move(normals);
			if (m_normals.size() != m_vertexCount)
				m_normals.resize(m_vertexCount, Float3::up);
			m_verticesUpdated = true;
		}
	}
	void Mesh::MoveTangents(std::vector<Float3>&& tangents)
	{
		// Take ownership of input data:
		if (&tangents != &m_tangents)
		{
			m_tangents = std::move(tangents);
			if (m_tangents.size() != m_vertexCount)
				m_tangents.resize(m_vertexCount, Float3::right);
			m_verticesUpdated = true;
		}
	}
	void Mesh::MoveColors(std::vector<Float4>&& colors)
	{
		// Take ownership of input data:
		if (&colors != &m_colors)
		{
			m_colors = std::move(colors);
			if (m_colors.size() != m_vertexCount)
				m_colors.resize(m_vertexCount, Float4::white);
			m_verticesUpdated = true;
		}
	}
	void Mesh::MoveUVs(std::vector<Float4>&& uvs)
	{
		// Take ownership of input data:
		if (&uvs != &m_uvs)
		{
			m_uvs = std::move(uvs);
			if (m_uvs.size() != m_vertexCount)
				m_uvs.resize(m_vertexCount, Float4::zero);
			m_verticesUpdated = true;
		}
	}
	void Mesh::MoveTriangles(std::vector<Uint3>&& triangles)
	{
		// Take ownership if input data:
		if (&triangles != &m_triangles)
		{
			m_triangleCount = static_cast<uint32_t>(triangles.size());
			m_triangles = std::move(triangles);
			m_indicesUpdated = true;
		}
	}



	// Getters:
	const std::string& Mesh::GetName() const
	{
		return m_name;
	}
	uint32_t Mesh::GetVertexCount() const
	{
		return m_vertexCount;
	}
	uint32_t Mesh::GetTriangleCount() const
	{
		return m_triangleCount;
	}
	std::vector<Float3>& Mesh::GetPositions()
	{
		return m_positions;
	}
	std::vector<Float3>& Mesh::GetNormals()
	{
		return m_normals;
	}
	std::vector<Float3>& Mesh::GetTangents()
	{
		return m_tangents;
	}
	std::vector<Float4>& Mesh::GetColors()
	{
		return m_colors;
	}
	std::vector<Float4>& Mesh::GetUVs()
	{
		return m_uvs;
	}
	std::vector<Uint3>& Mesh::GetTriangles()
	{
		return m_triangles;
	}
	void Mesh::RegisterUpdate()
	{
		m_vertexCount = m_positions.size();
		m_verticesUpdated = true;
	}
	emberBackendInterface::IMesh* Mesh::GetCopy(const std::string& newName)
	{
		Mesh* copy = new Mesh(newName);
		copy->SetPositions(m_positions);
		copy->SetNormals(m_normals);
		copy->SetTangents(m_tangents);
		copy->SetColors(m_colors);
		copy->SetUVs(m_uvs);
		copy->SetTriangles(m_triangles);
		return static_cast<emberBackendInterface::IMesh*>(copy);
	}



	// Mesh transformations (changes *this):
	void Mesh::ComputeNormals()
	{
		m_normals.resize(m_vertexCount, Float3::zero);
		for (auto& triangle : m_triangles)
		{
			Float3 p0 = m_positions[triangle[0]];
			Float3 p1 = m_positions[triangle[1]];
			Float3 p2 = m_positions[triangle[2]];
			Float3 edge10 = p1 - p0;
			Float3 edge20 = p2 - p0;
			Float3 normal = Float3::Cross(edge10, edge20).Normalize();

			m_normals[triangle[0]] += normal;
			m_normals[triangle[1]] += normal;
			m_normals[triangle[2]] += normal;
		}
		for (uint32_t i = 0; i < m_vertexCount; i++)
			m_normals[i].Normalize();
	}
	void Mesh::ComputeTangents()
	{
		if (m_uvs.size() != m_vertexCount)
		{
			LOG_WARN("Mesh '{}' has no uvs! Cannot compute tangents.", m_name);
			m_tangents.resize(m_vertexCount, Float3::zero);
			return;
		}
		if (m_normals.size() != m_vertexCount)
			ComputeNormals();

		m_tangents.resize(m_vertexCount, Float3::zero);
		for (auto& triangle : m_triangles)
		{
			Float3 p0 = m_positions[triangle[0]];
			Float3 p1 = m_positions[triangle[1]];
			Float3 p2 = m_positions[triangle[2]];
			Float3 edge10 = p1 - p0;
			Float3 edge20 = p2 - p0;
			Float2 uv0 = Float2(m_uvs[triangle[0]]);
			Float2 uv1 = Float2(m_uvs[triangle[1]]);
			Float2 uv2 = Float2(m_uvs[triangle[2]]);
			Float2 deltaUv10 = uv1 - uv0;
			Float2 deltaUv20 = uv2 - uv0;

			float denom = (deltaUv10.x * deltaUv20.y - deltaUv20.x * deltaUv10.y);
			float det = std::abs(denom) < 1e-6f ? 1.0f : 1.0f / denom;
			Float3 tangent = det * (deltaUv20.y * edge10 - deltaUv10.y * edge20);
			tangent.Normalize();

			m_tangents[triangle[0]] += tangent;
			m_tangents[triangle[1]] += tangent;
			m_tangents[triangle[2]] += tangent;
		}
		for (uint32_t i = 0; i < m_vertexCount; i++)
			m_tangents[i] = geometry3d::PointToPlaneProjection(m_tangents[i], Float3::zero, m_normals[i]).Normalize();
	}



	// Getters for backend only:
	uint32_t* Mesh::GetTrianglesUnrolled()
	{
		return reinterpret_cast<uint32_t*>(m_triangles.data());
	}
	uint32_t Mesh::GetSizeOfPositions() const
	{
		return m_vertexCount * 3 * sizeof(float);
	}
	uint32_t Mesh::GetSizeOfNormals() const
	{
		return m_vertexCount * 3 * sizeof(float);
	}
	uint32_t Mesh::GetSizeOfTangents() const
	{
		return m_vertexCount * 3 * sizeof(float);
	}
	uint32_t Mesh::GetSizeOfColors() const
	{
		return m_vertexCount * 4 * sizeof(float);
	}
	uint32_t Mesh::GetSizeOfUVs() const
	{
		return m_vertexCount * 4 * sizeof(float);
	}
	uint32_t Mesh::GetVertexBufferElementSize() const
	{
		return static_cast<uint32_t>(3 * 3 * sizeof(float) + 2 * 4 * sizeof(float));
	}
	uint64_t Mesh::GetVertexBufferSize() const
	{
		return static_cast<uint64_t>(GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors() + GetSizeOfUVs());
	}
	uint64_t Mesh::GetSizeOfTriangles() const
	{
		return static_cast<uint64_t>(m_triangles.size() * 3 * sizeof(uint32_t));
	}
	VertexBuffer* Mesh::GetVertexBuffer()
	{
		if (!m_isLoaded)
			Load();
		if (m_verticesUpdated)
		{
			UpdateVertexBuffer();
			m_verticesUpdated = false;
		}
		return m_pVertexBuffer.get();
	}
	IndexBuffer* Mesh::GetIndexBuffer()
	{
		if (!m_isLoaded)
			Load();
		if (m_indicesUpdated)
		{
			UpdateIndexBuffer();
			m_indicesUpdated = false;
		}
		return m_pIndexBuffer.get();
	}
	uint64_t Mesh::GetPositionsOffset() const
	{
		return 0;
	}
	uint64_t Mesh::GetNormalsOffset() const
	{
		return GetSizeOfPositions();
	}
	uint64_t Mesh::GetTangentsOffset() const
	{
		return GetSizeOfPositions() + GetSizeOfNormals();
	}
	uint64_t Mesh::GetColorsOffset() const
	{
		return GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents();
	}
	uint64_t Mesh::GetUVsOffset() const
	{
		return GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors();
	}
	IndexType Mesh::GetIndexType()
	{
		return IndexTypes::uint32;
	}
	bool Mesh::IsLoaded()
	{
		return m_isLoaded;
	}



	// Private methods:
	void Mesh::Load()
	{
		// Fill brocken vectors with default values:
		if (m_positions.size() != m_vertexCount)
			m_positions.resize(m_vertexCount, Float3::zero);
		if (m_normals.size() != m_vertexCount)
			m_normals.resize(m_vertexCount, Float3::up);
		if (m_tangents.size() != m_vertexCount)
			m_tangents.resize(m_vertexCount, Float3::right);
		if (m_colors.size() != m_vertexCount)
			m_colors.resize(m_vertexCount, Float4::white);
		if (m_uvs.size() != m_vertexCount)
			m_uvs.resize(m_vertexCount, Float4::zero);

		UpdateVertexBuffer();
		UpdateIndexBuffer();
		m_verticesUpdated = false;
		m_indicesUpdated = false;
		m_isLoaded = true;
	}
	void Mesh::Unload()
	{
		// Call destructors and set ptr to nullptrs:
		m_pVertexBuffer.reset();
		m_pIndexBuffer.reset();
		m_verticesUpdated = false;
		m_indicesUpdated = false;
		m_isLoaded = false;
	}
	#ifdef RESIZEABLE_BAR // No staging buffer:
	void Mesh::UpdateVertexBuffer()
	{
		// Set zero values if vectors not set:
		if (m_normals.size() != m_vertexCount)
			m_normals.resize(m_vertexCount, Float3::up);
		if (m_tangents.size() != m_vertexCount)
			m_tangents.resize(m_vertexCount, Float3::right);
		if (m_colors.size() != m_vertexCount)
			m_colors.resize(m_vertexCount, Float4::white);
		if (m_uvs.size() != m_vertexCount)
			m_uvs.resize(m_vertexCount, Float4::zero);

		if (m_isLoaded)	// wait for previous render calls to finish if mesh could be in use already
			vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);

		// Resize buffer if necessary:
		if (m_pVertexBuffer == nullptr || m_pVertexBuffer->GetCount() != m_vertexCount)
			m_pVertexBuffer = std::make_unique<VertexBuffer>(m_vertexCount, GetVertexBufferElementSize(), m_name);

		// Copy positions, colors, uvs:
		void* data;
		VKA(vmaMapMemory(Context::GetVmaAllocator(), m_pVertexBuffer->allocation, &data));
		memcpy(static_cast<char*>(data), m_positions.data(), GetSizeOfPositions());
		memcpy(static_cast<char*>(data) + GetSizeOfPositions(), m_normals.data(), GetSizeOfNormals());
		memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals(), m_tangents.data(), GetSizeOfTangents());
		memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents(), m_colors.data(), GetSizeOfColors());
		memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors(), m_uvs.data(), GetSizeOfUVs());
		VKA(vmaUnmapMemory(Context::GetVmaAllocator(), m_pVertexBuffer->allocation));
	}
	#else // With Staging buffer:
	void Mesh::UpdateVertexBuffer()
	{
		// Set zero values if vectors not set:
		if (m_normals.size() != m_vertexCount)
			m_normals.resize(m_vertexCount, Float3::up);
		if (m_tangents.size() != m_vertexCount)
			m_tangents.resize(m_vertexCount, Float3::right);
		if (m_colors.size() != m_vertexCount)
			m_colors.resize(m_vertexCount, Float4::white);
		if (m_uvs.size() != m_vertexCount)
			m_uvs.resize(m_vertexCount, Float4::zero);

		if (m_isLoaded)	// wait for previous render calls to finish if mesh could be in use already
			vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);

		// Resize buffer if necessary:
		if (m_pVertexBuffer == nullptr || m_pVertexBuffer->GetCount() != m_vertexCount)
			m_pVertexBuffer = std::make_unique<VertexBuffer>(m_vertexCount, GetVertexBufferElementSize(), m_name);

		// Copy: meshData -> stagingBuffer -> vertexBuffer
		StagingBuffer stagingBuffer(GetVertexBufferSize(), m_name);
		stagingBuffer.SetData(m_positions.data(), GetSizeOfPositions(), GetPositionsOffset());
		stagingBuffer.SetData(m_normals.data(), GetSizeOfNormals(), GetNormalsOffset());
		stagingBuffer.SetData(m_tangents.data(), GetSizeOfTangents(), GetTangentsOffset());
		stagingBuffer.SetData(m_colors.data(), GetSizeOfColors(), GetColorsOffset());
		stagingBuffer.SetData(m_uvs.data(), GetSizeOfUVs(), GetUVsOffset());
		stagingBuffer.UploadToBuffer(m_pVertexBuffer.get(), Context::GetLogicalDevice()->GetGraphicsQueue());
	}
	#endif
	#ifdef RESIZEABLE_BAR // No staging buffer:
	void Mesh::UpdateIndexBuffer()
	{
		if (m_isLoaded)	// wait for previous render calls to finish if mesh could be in use already
			vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);

		// Resize buffer if necessary:
		if (m_pIndexBuffer == nullptr || m_triangleCount != m_pIndexBuffer->GetCount())
			m_pIndexBuffer = std::make_unique<IndexBuffer>(m_triangleCount, 3 * (uint32_t)sizeof(uint32_t), m_name);

		// Copy triangle indexes:
		uint64_t size = GetSizeOfTriangles();
		void* data;
		VKA(vmaMapMemory(Context::GetVmaAllocator(), m_pIndexBuffer->allocation, &data));
		memcpy(data, GetTrianglesUnrolled(), size);
		VKA(vmaUnmapMemory(Context::GetVmaAllocator(), m_pIndexBuffer->allocation));
	}
	#else // With Staging buffer:
	void Mesh::UpdateIndexBuffer()
	{
		if (m_isLoaded)	// wait for previous render calls to finish if mesh could be in use already
			vkQueueWaitIdle(Context::GetLogicalDevice()->GetGraphicsQueue().queue);

		// Resize buffer if necessary:
		if (m_pIndexBuffer == nullptr || m_triangleCount != m_pIndexBuffer->GetCount())
			m_pIndexBuffer = std::make_unique<IndexBuffer>(m_triangleCount, 3 * (uint32_t)sizeof(uint32_t), m_name);

		// Copy: triangles -> stagingBuffer -> indexBuffer
		uint64_t size = GetSizeOfTriangles();
		StagingBuffer stagingBuffer(size, m_name);
		stagingBuffer.SetData(GetTrianglesUnrolled(), size);
		stagingBuffer.UploadToBuffer(m_pIndexBuffer.get(), Context::GetLogicalDevice()->GetGraphicsQueue());
	}
	#endif
}