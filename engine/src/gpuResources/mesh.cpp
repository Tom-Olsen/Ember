#include "mesh.h"
#include "indexBuffer.h"
#include "logger.h"
#include "stagingBuffer.h"
#include "vertexBuffer.h"
#include "vulkanContext.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Destructor:
	Mesh::Mesh(const std::string& name)
	{
		m_name = name;
	}
	Mesh::~Mesh()
	{

	}



	// Public methods:
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
			m_colors.resize(m_vertexCount, Float4::one);
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
		m_vertexBuffer.reset();
		m_indexBuffer.reset();
		m_verticesUpdated = false;
		m_indicesUpdated = false;
		m_isLoaded = false;
	}



	// Setters:
	void Mesh::SetName(const std::string& name)
	{
		m_name = name;
	}
	void Mesh::SetPositions(const std::vector<Float3>& positions)
	{
		// Copy values into member vector:
		m_vertexCount = static_cast<uint32_t>(positions.size());
		m_positions = positions;
		m_verticesUpdated = true;
	}
	void Mesh::SetNormals(const std::vector<Float3>& normals)
	{
		// Copy values into member vector and fill with default values if needed:
		m_normals = normals;
		if (normals.size() != m_vertexCount)
			m_normals.resize(m_vertexCount, Float3::up);
		m_verticesUpdated = true;
	}
	void Mesh::SetTangents(const std::vector<Float3>& tangents)
	{
		// Copy values into member vector and fill with default values if needed:
		m_tangents = tangents;
		if (tangents.size() != m_vertexCount)
			m_tangents.resize(m_vertexCount, Float3::right);
		m_verticesUpdated = true;
	}
	void Mesh::SetColors(const std::vector<Float4>& colors)
	{
		// Copy values into member vector and fill with default values if needed:
		m_colors = colors;
		if (colors.size() != m_vertexCount)
			m_colors.resize(m_vertexCount, Float4::one);
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
		m_uvs = uvs;
		if (uvs.size() != m_vertexCount)
			m_uvs.resize(m_vertexCount, Float4::zero);
		m_verticesUpdated = true;
	}
	void Mesh::SetTriangles(const std::vector<Uint3>& triangles)
	{
		// Copy values into member vector:
		m_triangleCount = static_cast<uint32_t>(triangles.size());
		m_triangles = triangles;
		m_indicesUpdated = true;
	}



	// Movers:
	void Mesh::MovePositions(std::vector<Float3>& positions)
	{
		// Take ownership if input data:
		if (&positions != &m_positions)
		{
			m_positions = std::move(positions);
			m_vertexCount = static_cast<uint32_t>(m_positions.size());
			m_verticesUpdated = true;
		}
	}
	void Mesh::MoveNormals(std::vector<Float3>& normals)
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
	void Mesh::MoveTangents(std::vector<Float3>& tangents)
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
	void Mesh::MoveColors(std::vector<Float4>& colors)
	{
		// Take ownership of input data:
		if (&colors != &m_colors)
		{
			m_colors = std::move(colors);
			if (m_colors.size() != m_vertexCount)
				m_colors.resize(m_vertexCount, Float4::one);
			m_verticesUpdated = true;
		}
	}
	void Mesh::MoveUVs(std::vector<Float4>& uvs)
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
	void Mesh::MoveTriangles(std::vector<Uint3>& triangles)
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
	uint32_t* Mesh::GetTrianglesUnrolled()
	{
		return reinterpret_cast<uint32_t*>(m_triangles.data());
	}
	uint32_t Mesh::GetSizeOfPositions() const
	{
		return m_vertexCount * sizeof(Float3);
	}
	uint32_t Mesh::GetSizeOfNormals() const
	{
		return m_vertexCount * sizeof(Float3);
	}
	uint32_t Mesh::GetSizeOfTangents() const
	{
		return m_vertexCount * sizeof(Float3);
	}
	uint32_t Mesh::GetSizeOfColors() const
	{
		return m_vertexCount * sizeof(Float4);
	}
	uint32_t Mesh::GetSizeOfUVs() const
	{
		return m_vertexCount * sizeof(Float4);
	}
	uint32_t Mesh::GetVertexBufferElementSize() const
	{
		return static_cast<uint32_t>(3 * sizeof(Float3) + 2 * sizeof(Float4));
	}
	uint64_t Mesh::GetVertexBufferSize() const
	{
		return static_cast<uint64_t>(GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors() + GetSizeOfUVs());
	}
	uint64_t Mesh::GetSizeOfTriangles() const
	{
		return static_cast<uint64_t>(m_triangles.size() * sizeof(Uint3));
	}
	VkDeviceSize Mesh::GetPositionsOffset() const
	{
		return 0;
	}
	VkDeviceSize Mesh::GetNormalsOffset() const
	{
		return GetSizeOfPositions();
	}
	VkDeviceSize Mesh::GetTangentsOffset() const
	{
		return GetSizeOfPositions() + GetSizeOfNormals();
	}
	VkDeviceSize Mesh::GetColorsOffset() const
	{
		return GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents();
	}
	VkDeviceSize Mesh::GetUVsOffset() const
	{
		return GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors();
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
		return m_vertexBuffer.get();
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
		return m_indexBuffer.get();
	}
	bool Mesh::IsLoaded()
	{
		return m_isLoaded;
	}
	Mesh* Mesh::GetCopy(const std::string& newName)
	{
		Mesh* copy = new Mesh(newName);
		copy->SetPositions(m_positions);
		copy->SetNormals(m_normals);
		copy->SetTangents(m_tangents);
		copy->SetColors(m_colors);
		copy->SetUVs(m_uvs);
		copy->SetTriangles(m_triangles);
		return copy;
	}



	// Mesh transformations (changes *this!):
	Mesh* Mesh::Translate(const Float3& translation)
	{
		for (Float3& position : m_positions)
			position += translation;
		m_verticesUpdated = true;
		return this;
	}
	Mesh* Mesh::Rotate(const Float3x3& rotation)
	{
		// Positions:
		for (uint32_t i = 0; i < m_vertexCount; i++)
			m_positions[i] = rotation * m_positions[i];

		// Normals:
		if (m_normals.size() == m_vertexCount)
			for (uint32_t i = 0; i < m_vertexCount; i++)
				m_normals[i] = rotation * m_normals[i];

		// Tangents:
		if (m_tangents.size() == m_vertexCount)
			for (uint32_t i = 0; i < m_vertexCount; i++)
				m_tangents[i] = rotation * m_tangents[i];

		m_verticesUpdated = true;
		return this;
	}
	Mesh* Mesh::Rotate(const Float4x4& rotation)
	{
		Float3x3 rotation3x3 = Float3x3(rotation);

		// Positions:
		for (uint32_t i = 0; i < m_vertexCount; i++)
			m_positions[i] = rotation3x3 * m_positions[i];

		// Normals:
		if (m_normals.size() == m_vertexCount)
			for (uint32_t i = 0; i < m_vertexCount; i++)
				m_normals[i] = rotation3x3 * m_normals[i];

		// Tangents:
		if (m_tangents.size() == m_vertexCount)
			for (uint32_t i = 0; i < m_vertexCount; i++)
				m_tangents[i] = rotation3x3 * m_tangents[i];

		m_verticesUpdated = true;
		return this;
	}
	Mesh* Mesh::Scale(const Float3& scale)
	{
		Float3 invScale = 1.0 / scale;

		// Positions:
		for (uint32_t i = 0; i < m_vertexCount; i++)
			m_positions[i] = scale * m_positions[i];

		// Normals:
		if (m_normals.size() == m_vertexCount)
			for (uint32_t i = 0; i < m_vertexCount; i++)
				m_normals[i] = (invScale * m_normals[i]).Normalize();

		// Tangents:
		if (m_tangents.size() == m_vertexCount)
			for (uint32_t i = 0; i < m_vertexCount; i++)
				m_tangents[i] = (invScale * m_tangents[i]).Normalize();

		m_verticesUpdated = true;
		return this;
	}
	Mesh* Mesh::Scale(float scale)
	{
		return Scale(Float3(scale));
	}
	Mesh* Mesh::Transform(const Float4x4& transformationMatrix)
	{
		Float4x4 normalMatrix = transformationMatrix.Inverse().Transpose();

		// Positions:
		for (uint32_t i = 0; i < m_vertexCount; i++)
			m_positions[i] = Float3(transformationMatrix * Float4(m_positions[i], 1.0f));

		// Normals:
		if (m_normals.size() == m_vertexCount)
			for (uint32_t i = 0; i < m_vertexCount; i++)
				m_normals[i] = Float3(normalMatrix * Float4(m_normals[i], 0.0f)).Normalize();

		// Tangents:
		if (m_tangents.size() == m_vertexCount)
			for (uint32_t i = 0; i < m_vertexCount; i++)
				m_tangents[i] = Float3(normalMatrix * Float4(m_tangents[i], 0.0f)).Normalize();

		m_verticesUpdated = true;
		return this;
	}
	Mesh* Mesh::Subdivide()
	{
		std::vector<Uint3> newTriangles;
		newTriangles.reserve(4 * m_triangleCount);

		// Subdivide each triangle:
		uint32_t newVertexIndex = m_vertexCount;
		for (uint32_t i = 0; i < m_triangleCount; i++)
		{
			Uint3 triangle = m_triangles[i];

			// Add positions:
			Float3 positionA = 0.5f * (m_positions[triangle[0]] + m_positions[triangle[1]]);
			Float3 positionB = 0.5f * (m_positions[triangle[1]] + m_positions[triangle[2]]);
			Float3 positionC = 0.5f * (m_positions[triangle[2]] + m_positions[triangle[0]]);
			m_positions.push_back(positionA);
			m_positions.push_back(positionB);
			m_positions.push_back(positionC);

			// Add normals:
			Float3 normalA = (m_normals[triangle[0]] + m_normals[triangle[1]]).Normalize();
			Float3 normalB = (m_normals[triangle[1]] + m_normals[triangle[2]]).Normalize();
			Float3 normalC = (m_normals[triangle[2]] + m_normals[triangle[0]]).Normalize();
			m_normals.push_back(normalA);
			m_normals.push_back(normalB);
			m_normals.push_back(normalC);

			// Add tangents:
			Float3 tangentA = (m_tangents[triangle[0]] + m_tangents[triangle[1]]).Normalize();
			Float3 tangentB = (m_tangents[triangle[1]] + m_tangents[triangle[2]]).Normalize();
			Float3 tangentC = (m_tangents[triangle[2]] + m_tangents[triangle[0]]).Normalize();
			m_tangents.push_back(tangentA);
			m_tangents.push_back(tangentB);
			m_tangents.push_back(tangentC);

			// Add colors:
			Float4 colorA = 0.5f * (m_colors[triangle[0]] + m_colors[triangle[1]]);
			Float4 colorB = 0.5f * (m_colors[triangle[1]] + m_colors[triangle[2]]);
			Float4 colorC = 0.5f * (m_colors[triangle[2]] + m_colors[triangle[0]]);
			m_colors.push_back(colorA);
			m_colors.push_back(colorB);
			m_colors.push_back(colorC);

			// Add Uvs:
			Float4 uvA = 0.5f * (m_uvs[triangle[0]] + m_uvs[triangle[1]]);
			Float4 uvB = 0.5f * (m_uvs[triangle[1]] + m_uvs[triangle[2]]);
			Float4 uvC = 0.5f * (m_uvs[triangle[2]] + m_uvs[triangle[0]]);
			m_uvs.push_back(uvA);
			m_uvs.push_back(uvB);
			m_uvs.push_back(uvC);

			// Add 4 triangles:
			Uint3 newTriangleA = { triangle[0], newVertexIndex, newVertexIndex + 2 };
			Uint3 newTriangleB = { newVertexIndex, newVertexIndex + 1, newVertexIndex + 2 };
			Uint3 newTriangleC = { newVertexIndex, triangle[1], newVertexIndex + 1 };
			Uint3 newTriangleD = { newVertexIndex + 2, newVertexIndex + 1, triangle[2] };
			newTriangles.push_back(newTriangleA);
			newTriangles.push_back(newTriangleB);
			newTriangles.push_back(newTriangleC);
			newTriangles.push_back(newTriangleD);

			newVertexIndex += 3;
		}

		// Update mesh data:
		m_verticesUpdated = true;
		m_vertexCount = static_cast<uint32_t>(m_positions.size());
		MoveTriangles(newTriangles);
		return this;
	}
	Mesh* Mesh::Spherify(float factor, float radius)
	{
		factor = std::clamp(factor, 0.0f, 1.0f);
		radius = std::max(1e-8f, radius);

		for (uint32_t i = 0; i < m_vertexCount; i++)
		{
			Float3 sphereNormal = m_positions[i].Normalize();
			m_positions[i] = radius * (m_positions[i] + factor * (sphereNormal - m_positions[i]));
			m_normals[i] = (m_normals[i] + factor * (sphereNormal - m_normals[i])).Normalize();
			m_tangents[i] = geometry3d::PointToPlaneProjection(m_tangents[i], Float3::zero, m_normals[i]).Normalize();
		}

		m_verticesUpdated = true;
		return this;
	}
	Mesh* Mesh::InvertFaces()
	{
		// Triangles:
		for (Uint3& triangle : m_triangles)
			std::swap(triangle[1], triangle[2]);

		// Normals:
		if (m_normals.size() == m_vertexCount)
			for (Float3& normal : m_normals)
				normal = -normal;

		// Tangents:
		if (m_tangents.size() == m_vertexCount)
			for (Float3& tangent : m_tangents)
				tangent = -tangent;

		return this;
	}
	Mesh* Mesh::RescaleUVs(const Float4& scale, const Float4& offset)
	{
		for (Float4& uv : m_uvs)
			uv = scale * uv + offset;
		return this;
	}
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



	// Static methods:
	VkIndexType Mesh::GetIndexType()
	{
		return VK_INDEX_TYPE_UINT32;
	}
	Mesh* Mesh::Merge(std::vector<Mesh*>& meshes, const std::string& name)
	{
		// Get total vertex and triangle counts:
		uint32_t vertexCount = 0;
		uint32_t triangleCount = 0;
		for (const Mesh* mesh : meshes)
		{
			vertexCount += mesh->GetVertexCount();
			triangleCount += mesh->GetTriangleCount();
		}

		// Triangle reindexing:
		uint32_t index = 0;
		uint32_t vertCount = 0;

		// Prepare merged data:
		std::vector<Float3> mergedPositions;
		std::vector<Float3> mergedNormals;
		std::vector<Float3> mergedTangents;
		std::vector<Float4> mergedColors;
		std::vector<Float4> mergedUvs;
		std::vector<Uint3> mergedTriangles;
		mergedPositions.reserve(vertexCount);
		mergedNormals.reserve(vertexCount);
		mergedTangents.reserve(vertexCount);
		mergedColors.reserve(vertexCount);
		mergedUvs.reserve(vertexCount);
		mergedTriangles.reserve(triangleCount);

		for (Mesh* mesh : meshes)
		{
			// Get data of current mesh:

			// Append positions (must always be present).
			std::vector<Float3> positions = mesh->GetPositions();
			mergedPositions.insert(mergedPositions.end(), positions.begin(), positions.end());

			// Append normals:
			std::vector<Float3> normals = mesh->GetNormals();
			mergedNormals.insert(mergedNormals.end(), normals.begin(), normals.end());


			// Append tangents:
			std::vector<Float3> m_tangents = mesh->GetTangents();
			mergedTangents.insert(mergedTangents.end(), m_tangents.begin(), m_tangents.end());

			// Append colors:
			std::vector<Float4> colors = mesh->GetColors();
			mergedColors.insert(mergedColors.end(), colors.begin(), colors.end());

			// Append uvs:
			std::vector<Float4> uvs = mesh->GetUVs();
			mergedUvs.insert(mergedUvs.end(), uvs.begin(), uvs.end());

			// Handle triangles:
			std::vector<Uint3> triangles = mesh->GetTriangles();
			for (int i = 0; i < triangles.size(); i++)
			{
				mergedTriangles.push_back(triangles[i] + Uint3(vertCount));
				index++;
			}
			vertCount += mesh->GetVertexCount();
		}

		// Construct mergedMesh:
		Mesh* mergedMesh = new Mesh(name);
		mergedMesh->MovePositions(mergedPositions);
		mergedMesh->MoveNormals(mergedNormals);
		mergedMesh->MoveTangents(mergedTangents);
		mergedMesh->MoveColors(mergedColors);
		mergedMesh->MoveUVs(mergedUvs);
		mergedMesh->MoveTriangles(mergedTriangles);
		return mergedMesh;
	}



	// Debugging:
	std::string Mesh::ToString()
	{
		std::string str = "Mesh:\n";

		str += "Positions:\n";
		for (const Float3& position : m_positions)
			str += position.ToString() + "\n";

		str += "Normals:";
		if (m_normals.size() != m_vertexCount)
			str += " invalid\n";
		else
		{
			str += "\n";
			for (const Float3& normal : m_normals)
				str += normal.ToString() + "\n";
		}

		str += "Tangents:";
		if (m_tangents.size() != m_vertexCount)
			str += " invalid\n";
		else
		{
			str += "\n";
			for (const Float3& tangent : m_tangents)
				str += tangent.ToString() + "\n";
		}

		str += "Colors:";
		if (m_colors.size() != m_vertexCount)
			str += " invalid\n";
		else
		{
			str += "\n";
			for (const Float4& color : m_colors)
				str += color.ToString() + "\n";
		}

		str += "Uvs:";
		if (m_uvs.size() != m_vertexCount)
			str += " invalid\n";
		else
		{
			str += "\n";
			for (const Float4& uv : m_uvs)
				str += uv.ToString() + "\n";
		}

		str += "Triangles:\n";
		for (const Uint3& triangle : m_triangles)
			str += triangle.ToString() + "\n";

		str += "\n";
		return str;
	}



	// Private:
	#ifdef RESIZEABLE_BAR // No staging buffer:
	void Mesh::UpdateVertexBuffer()
	{
		// Set zero values if vectors not set:
		if (m_normals.size() != m_vertexCount)
			m_normals.resize(m_vertexCount, Float3::zero);
		if (m_tangents.size() != m_vertexCount)
			m_tangents.resize(m_vertexCount, Float3::zero);
		if (m_colors.size() != m_vertexCount)
			m_colors.resize(m_vertexCount, Float4::zero);
		if (m_uvs.size() != m_vertexCount)
			m_uvs.resize(m_vertexCount, Float4::zero);

		if (m_isLoaded)	// wait for previous render calls to finish if mesh could be in use already
			vkQueueWaitIdle(Context::logicalDevice.GetGraphicsQueue().queue);

		// Resize buffer if necessary:
		if (m_vertexBuffer == nullptr || m_vertexBuffer->GetCount() != m_vertexCount)
			m_vertexBuffer = std::make_unique<VertexBuffer>(m_vertexCount, GetVertexBufferElementSize(), m_name);

		// Copy positions, colors, uvs:
		void* data;
		VKA(vmaMapMemory(Context::GetVmaAllocator(), m_vertexBuffer->allocation, &data));
		memcpy(static_cast<char*>(data), m_positions.data(), GetSizeOfPositions());
		memcpy(static_cast<char*>(data) + GetSizeOfPositions(), m_normals.data(), GetSizeOfNormals());
		memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals(), m_tangents.data(), GetSizeOfTangents());
		memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents(), m_colors.data(), GetSizeOfColors());
		memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors(), m_uvs.data(), GetSizeOfUVs());
		VKA(vmaUnmapMemory(Context::GetVmaAllocator(), m_vertexBuffer->allocation));
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
			m_colors.resize(m_vertexCount, Float4::one);
		if (m_uvs.size() != m_vertexCount)
			m_uvs.resize(m_vertexCount, Float4::zero);

		if (m_isLoaded)	// wait for previous render calls to finish if mesh could be in use already
			vkQueueWaitIdle(Context::logicalDevice.GetGraphicsQueue().queue);

		// Resize buffer if necessary:
		if (m_vertexBuffer == nullptr || m_vertexBuffer->GetCount() != m_vertexCount)
			m_vertexBuffer = std::make_unique<VertexBuffer>(m_vertexCount, GetVertexBufferElementSize(), m_name);

		// Copy: meshData -> stagingBuffer -> vertexBuffer
		StagingBuffer stagingBuffer(GetVertexBufferSize(), m_name);
		stagingBuffer.SetData(m_positions.data(), GetSizeOfPositions(), GetPositionsOffset());
		stagingBuffer.SetData(m_normals.data(), GetSizeOfNormals(), GetNormalsOffset());
		stagingBuffer.SetData(m_tangents.data(), GetSizeOfTangents(), GetTangentsOffset());
		stagingBuffer.SetData(m_colors.data(), GetSizeOfColors(), GetColorsOffset());
		stagingBuffer.SetData(m_uvs.data(), GetSizeOfUVs(), GetUVsOffset());
		stagingBuffer.UploadToBuffer(m_vertexBuffer.get(), Context::logicalDevice.GetGraphicsQueue());
	}
	#endif
	#ifdef RESIZEABLE_BAR // No staging buffer:
	void Mesh::UpdateIndexBuffer()
	{
		if (m_isLoaded)	// wait for previous render calls to finish if mesh could be in use already
			vkQueueWaitIdle(Context::logicalDevice.GetGraphicsQueue().queue);

		// Resize buffer if necessary:
		if (m_indexBuffer == nullptr || m_triangleCount != m_indexBuffer->GetCount())
			m_indexBuffer = std::make_unique<IndexBuffer>(m_triangleCount, (uint32_t)sizeof(Uint3), m_name);

		// Copy triangle indexes:
		uint64_t size = GetSizeOfTriangles();
		void* data;
		VKA(vmaMapMemory(Context::GetVmaAllocator(), m_indexBuffer->allocation, &data));
		memcpy(data, GetTrianglesUnrolled(), size);
		VKA(vmaUnmapMemory(Context::GetVmaAllocator(), m_indexBuffer->allocation));
	}
	#else // With Staging buffer:
	void Mesh::UpdateIndexBuffer()
	{
		if (m_isLoaded)	// wait for previous render calls to finish if mesh could be in use already
			vkQueueWaitIdle(Context::logicalDevice.GetGraphicsQueue().queue);

		// Resize buffer if necessary:
		if (m_indexBuffer == nullptr || m_triangleCount != m_indexBuffer->GetCount())
			m_indexBuffer = std::make_unique<IndexBuffer>(m_triangleCount, (uint32_t)sizeof(Uint3), m_name);

		// Copy: triangles -> stagingBuffer -> indexBuffer
		uint64_t size = GetSizeOfTriangles();
		StagingBuffer stagingBuffer(size, m_name);
		stagingBuffer.SetData(GetTrianglesUnrolled(), size);
		stagingBuffer.UploadToBuffer(m_indexBuffer.get(), Context::logicalDevice.GetGraphicsQueue());
	}
	#endif
}