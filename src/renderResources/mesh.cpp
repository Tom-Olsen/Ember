#include "mesh.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"



// Constructor/Destructor:
Mesh::Mesh(const std::string& name)
{
	m_name = name;
}
Mesh::~Mesh()
{

}



// Public methods:
void Mesh::Load(VulkanContext* context)
{
	UpdateVertexBuffer(context);
	UpdateIndexBuffer(context);
	m_verticesUpdated = false;
	m_indicesUpdated = false;
	m_isLoaded = true;
}
void Mesh::Unload()
{
	// call destructors and set ptr to nullptrs:
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
	m_vertexCount = static_cast<uint32_t>(positions.size());
	m_positions = positions;
	m_verticesUpdated = true;
}
void Mesh::SetNormals(const std::vector<Float3>& normals)
{
	if (normals.size() == m_vertexCount)
		m_normals = normals;
	else
	{
		LOG_WARN("Mesh '{}' normals size does not match vertex count! Setting all normals to zero.", m_name);
		m_normals.clear();
		m_normals.resize(m_vertexCount, Float3());
	}
	m_verticesUpdated = true;
}
void Mesh::SetTangents(const std::vector<Float3>& tangents)
{
	if (tangents.size() == m_vertexCount)
		m_tangents = tangents;
	else
	{
		LOG_WARN("Mesh '{}' tangents size does not match vertex count! Setting all tangents to zero.", m_name);
		m_tangents.clear();
		m_tangents.resize(m_vertexCount, Float3());
	}
	m_verticesUpdated = true;
}
void Mesh::SetColors(const std::vector<Float4>& colors)
{
	if (colors.size() == m_vertexCount)
		m_colors = colors;
	else
	{
		LOG_WARN("Mesh '{}' colors size does not match vertex count! Setting all colors to zero.", m_name);
		m_colors.clear();
		m_colors.resize(m_vertexCount, Float4());
	}
	m_verticesUpdated = true;
}
void Mesh::SetUniformColor(const Float4& color)
{
	if (m_colors.size() == m_vertexCount)
	{
		for (Float4& col : m_colors)
			col = color;
	}
	else
	{
		m_colors.clear();
		m_colors.resize(m_vertexCount, color);
	}
	m_verticesUpdated = true;
}
void Mesh::SetUVs(const std::vector<Float4>& uvs)
{
	if (uvs.size() == m_vertexCount)
		m_uvs = uvs;
	else
	{
		LOG_WARN("Mesh '{}' uvs size does not match vertex count! Setting all uvs to zero.", m_name);
		m_uvs.clear();
		m_uvs.resize(m_vertexCount, Float4());
	}
	m_verticesUpdated = true;
}
void Mesh::SetTriangles(const std::vector<Uint3>& triangles)
{
	m_triangleCount = static_cast<uint32_t>(triangles.size());
	m_triangles = triangles;
	m_indicesUpdated = true;
}



// Movers:
void Mesh::MovePositions(std::vector<Float3>& positions)
{
	m_vertexCount = static_cast<uint32_t>(positions.size());
	m_positions = std::move(positions);
	m_verticesUpdated = true;
}
void Mesh::MoveNormals(std::vector<Float3>& normals)
{
	if (normals.size() == m_vertexCount)
		m_normals = std::move(normals);
	else
	{
		LOG_WARN("Mesh '{}' normals size does not match vertex count! Setting all normals to zero.", m_name);
		m_normals.clear();
		m_normals.resize(m_vertexCount, Float3());
	}
	m_verticesUpdated = true;
}
void Mesh::MoveTangents(std::vector<Float3>& tangents)
{
	if (tangents.size() == m_vertexCount)
		m_tangents = std::move(tangents);
	else
	{
		LOG_WARN("Mesh '{}' tangents size does not match vertex count! Setting all tangents to zero.", m_name);
		m_tangents.clear();
		m_tangents.resize(m_vertexCount, Float3());
	}
	m_verticesUpdated = true;
}
void Mesh::MoveColors(std::vector<Float4>& colors)
{
	if (colors.size() == m_vertexCount)
		m_colors = std::move(colors);
	else
	{
		LOG_WARN("Mesh '{}' colors size does not match vertex count! Setting all colors to zero.", m_name);
		m_colors.clear();
		m_colors.resize(m_vertexCount, Float4());
	}
	m_verticesUpdated = true;
}
void Mesh::MoveUVs(std::vector<Float4>& uvs)
{
	if (uvs.size() == m_vertexCount)
		m_uvs = std::move(uvs);
	else
	{
		LOG_WARN("Mesh '{}' uvs size does not match vertex count! Setting all uvs to zero.", m_name);
		m_uvs.clear();
		m_uvs.resize(m_vertexCount, Float4());
	}
	m_verticesUpdated = true;
}
void Mesh::MoveTriangles(std::vector<Uint3>& triangles)
{
	m_triangleCount = static_cast<uint32_t>(triangles.size());
	m_triangles = std::move(triangles);
	m_indicesUpdated = true;
}



// Getters:
std::string Mesh::GetName() const
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
	return static_cast<uint32_t>(m_positions.size()) * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfNormals() const
{
	return static_cast<uint32_t>(m_normals.size()) * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfTangents() const
{
	return static_cast<uint32_t>(m_tangents.size()) * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfColors() const
{
	return static_cast<uint32_t>(m_colors.size()) * sizeof(Float4);
}
uint32_t Mesh::GetSizeOfUVs() const
{
	return static_cast<uint32_t>(m_uvs.size()) * sizeof(Float4);
}
uint32_t Mesh::GetSizeOfTriangles() const
{
	return static_cast<uint32_t>(m_triangles.size()) * sizeof(Uint3);
}
uint32_t Mesh::GetBindingCount()
{
	return 5;
}
VkBuffer* Mesh::GetBuffers(VulkanContext* context)
{
	if (!m_isLoaded)
		Load(context);
	m_buffers[0] = m_vertexBuffer->GetVkBuffer();
	m_buffers[1] = m_vertexBuffer->GetVkBuffer();
	m_buffers[2] = m_vertexBuffer->GetVkBuffer();
	m_buffers[3] = m_vertexBuffer->GetVkBuffer();
	m_buffers[4] = m_vertexBuffer->GetVkBuffer();
	return m_buffers;
}
VkDeviceSize* Mesh::GetOffsets()
{
	m_offsets[0] = 0;
	m_offsets[1] = GetSizeOfPositions();
	m_offsets[2] = GetSizeOfPositions() + GetSizeOfNormals();
	m_offsets[3] = GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents();
	m_offsets[4] = GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors();
	return m_offsets;
}
VmaBuffer* Mesh::GetVertexBuffer(VulkanContext* context)
{
	if (!m_isLoaded)
		Load(context);
	if (m_verticesUpdated && context != nullptr)
	{
		UpdateVertexBuffer(context);
		m_verticesUpdated = false;
	}
	return m_vertexBuffer.get();
}
VmaBuffer* Mesh::GetIndexBuffer(VulkanContext* context)
{
	if (!m_isLoaded)
		Load(context);
	if (m_indicesUpdated && context != nullptr)
	{
		UpdateIndexBuffer(context);
		m_indicesUpdated = false;
	}
	return m_indexBuffer.get();
}
bool Mesh::IsLoaded()
{
	return m_isLoaded;
}
bool Mesh::HasNormals()
{
	return m_normals.size() == m_vertexCount;
}
bool Mesh::HasTangents()
{
	return m_tangents.size() == m_vertexCount;
}
bool Mesh::HasColors()
{
	return m_colors.size() == m_vertexCount;
}
bool Mesh::HasUVs()
{
	return m_uvs.size() == m_vertexCount;
}
Mesh* Mesh::GetCopy(const std::string& newName)
{
	Mesh* copy = new Mesh(newName);
	copy->SetPositions(m_positions);
	if (HasNormals())
		copy->SetNormals(m_normals);
	if (HasTangents())
		copy->SetTangents(m_tangents);
	if (HasColors())
		copy->SetColors(m_colors);
	if (HasUVs())
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
	bool hasNormals = HasNormals();
	bool hasTangents = HasTangents();
	for (uint32_t i = 0; i < m_vertexCount; i++)
	{
		m_positions[i] = rotation * m_positions[i];
		if (hasNormals)
			m_normals[i] = rotation * m_normals[i];
		if (hasTangents)
			m_tangents[i] = rotation * m_tangents[i];
	}
	m_verticesUpdated = true;
	return this;
}
Mesh* Mesh::Rotate(const Float4x4& rotation)
{
	Float3x3 rotation3x3 = Float3x3(rotation);
	bool hasNormals = HasNormals();
	bool hasTangents = HasTangents();
	for (uint32_t i = 0; i < m_vertexCount; i++)
	{
		m_positions[i] = rotation3x3 * m_positions[i];
		if (hasNormals)
			m_normals[i] = rotation3x3 * m_normals[i];
		if (hasTangents)
			m_tangents[i] = rotation3x3 * m_tangents[i];
	}
	m_verticesUpdated = true;
	return this;
}
Mesh* Mesh::Scale(const Float3& scale)
{
	for (Float3& position : m_positions)
		position *= scale;
	m_verticesUpdated = true;
	return this;
}
Mesh* Mesh::Scale(float scale)
{
	return Scale(Float3(scale));
}
Mesh* Mesh::Subdivide()
{
	std::vector<Uint3> newTriangles;
	newTriangles.reserve(4 * m_triangleCount);
	bool hasNormals = HasNormals();
	bool hasTangents = HasTangents();
	bool hasColors = HasColors();
	bool hasUVs = HasUVs();

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
		if (hasNormals)
		{
			Float3 normalA = (m_normals[triangle[0]] + m_normals[triangle[1]]).Normalize();
			Float3 normalB = (m_normals[triangle[1]] + m_normals[triangle[2]]).Normalize();
			Float3 normalC = (m_normals[triangle[2]] + m_normals[triangle[0]]).Normalize();
			m_normals.push_back(normalA);
			m_normals.push_back(normalB);
			m_normals.push_back(normalC);
		}

		// Add tangents:
		if (hasTangents)
		{
			Float3 tangentA = (m_tangents[triangle[0]] + m_tangents[triangle[1]]).Normalize();
			Float3 tangentB = (m_tangents[triangle[1]] + m_tangents[triangle[2]]).Normalize();
			Float3 tangentC = (m_tangents[triangle[2]] + m_tangents[triangle[0]]).Normalize();
			m_tangents.push_back(tangentA);
			m_tangents.push_back(tangentB);
			m_tangents.push_back(tangentC);
		}

		// Add colors:
		if (hasColors)
		{
			Float4 colorA = 0.5f * (m_colors[triangle[0]] + m_colors[triangle[1]]);
			Float4 colorB = 0.5f * (m_colors[triangle[1]] + m_colors[triangle[2]]);
			Float4 colorC = 0.5f * (m_colors[triangle[2]] + m_colors[triangle[0]]);
			m_colors.push_back(colorA);
			m_colors.push_back(colorB);
			m_colors.push_back(colorC);
		}

		// Add Uvs:
		if (hasUVs)
		{
			Float4 uvA = 0.5f * (m_uvs[triangle[0]] + m_uvs[triangle[1]]);
			Float4 uvB = 0.5f * (m_uvs[triangle[1]] + m_uvs[triangle[2]]);
			Float4 uvC = 0.5f * (m_uvs[triangle[2]] + m_uvs[triangle[0]]);
			m_uvs.push_back(uvA);
			m_uvs.push_back(uvB);
			m_uvs.push_back(uvC);
		}
		
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

	// Update mesh data (forces bool updates and logic):
	MovePositions(m_positions);
	if (hasNormals)
		MoveNormals(m_normals);
	if (hasTangents)
		MoveTangents(m_tangents);
	if (hasColors)
		MoveColors(m_colors);
	if (hasUVs)
		MoveUVs(m_uvs);
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
		if (HasNormals())
			m_normals[i] = (m_normals[i] + factor * (sphereNormal - m_normals[i])).Normalize();
		if (HasTangents())
			m_tangents[i] = Float3::VectorToPlaneProjection(m_tangents[i], m_normals[i]).Normalize();
	}

	// Update mesh data (forces bool updates and logic):
	MovePositions(m_positions);
	if (HasNormals())
		MoveNormals(m_normals);
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
	if (!HasNormals())
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
		m_tangents[i] = Float3::VectorToPlaneProjection(m_tangents[i], m_normals[i]).Normalize();
}



// Static binding descriptions:
std::vector<VkVertexInputBindingDescription> Mesh::GetBindingDescription()
{
	// binding = index of the binding in the array of bindings
	// stride = number of bytes from one entry to the next
	// inputRate = VK_VERTEX_INPUT_RATE_VERTEX : move to the next data entry after each vertex
	//             VK_VERTEX_INPUT_RATE_INSTANCE : move to the next data entry after each instance
	// => x0y0z0, x1y1z1, ..., nxyz0, nxyz1, ..., r0g0b0a0, r1g1b1a1, ...
	std::vector<VkVertexInputBindingDescription> bindingDescriptions =
	{
		GetPositionBindingDescription(),
		GetNormalBindingDescription(),
		GetTangentBindingDescription(),
		GetColorBindingDescription(),
		GetUvBindingDescription(0)
	};
	return bindingDescriptions;
}
VkVertexInputBindingDescription Mesh::GetPositionBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Float3);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}
VkVertexInputBindingDescription Mesh::GetNormalBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 1;
	bindingDescription.stride = sizeof(Float3);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}
VkVertexInputBindingDescription Mesh::GetTangentBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 2;
	bindingDescription.stride = sizeof(Float3);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}
VkVertexInputBindingDescription Mesh::GetColorBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 3;
	bindingDescription.stride = sizeof(Float4);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}
VkVertexInputBindingDescription Mesh::GetUvBindingDescription(uint32_t channel)
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 4 + channel;
	bindingDescription.stride = sizeof(Float4);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}



// Static attribute descriptions:
std::vector<VkVertexInputAttributeDescription> Mesh::GetAttributeDescriptions()
{
	// binding = index of the attribute in the array of attributes
	// location = reference to the location directive of the input in the vertex shader
	// format = how to interpret the data
	// offset = data offset
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions =
	{
		GetPositionAttributeDescription(),
		GetNormalAttributeDescription(),
		GetTangentAttributeDescription(),
		GetColorAttributeDescription(),
		GetUvAttributeDescription(0)
	};
	return attributeDescriptions;
}
VkVertexInputAttributeDescription Mesh::GetPositionAttributeDescription()
{
	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 0;
	attributeDescription.location = 0;
	attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescription.offset = 0;
	return attributeDescription;
}
VkVertexInputAttributeDescription Mesh::GetNormalAttributeDescription()
{
	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 1;
	attributeDescription.location = 1;
	attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescription.offset = 0;
	return attributeDescription;
}
VkVertexInputAttributeDescription Mesh::GetTangentAttributeDescription()
{
	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 2;
	attributeDescription.location = 2;
	attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescription.offset = 0;
	return attributeDescription;
}
VkVertexInputAttributeDescription Mesh::GetColorAttributeDescription()
{
	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 3;
	attributeDescription.location = 3;
	attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescription.offset = 0;
	return attributeDescription;
}
VkVertexInputAttributeDescription Mesh::GetUvAttributeDescription(uint32_t channel)
{
	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 4 + channel;
	attributeDescription.location = 4 + channel;
	attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescription.offset = 0;
	return attributeDescription;
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

	// Check if any mesh has an optional channel:
	bool hasNormals = false;
	bool hasTangents = false;
	bool hasColors = false;
	bool HasUVs = false;
	for (Mesh* mesh : meshes)
	{
		if (mesh->HasNormals())
			hasNormals = true;
		if (mesh->HasTangents())
			hasTangents = true;
		if (mesh->HasColors())
			hasColors = true;
		if (mesh->HasUVs())
			HasUVs = true;
	}


	for (Mesh* mesh : meshes)
	{
		// Get data of current mesh:

		// Append positions (must always be present).
		std::vector<Float3> positions = mesh->GetPositions();
		mergedPositions.insert(mergedPositions.end(), positions.begin(), positions.end());

		// Append normals:
		if (hasNormals)
		{
			std::vector<Float3> normals = mesh->GetNormals();
			mergedNormals.insert(mergedNormals.end(), normals.begin(), normals.end());
		}

		// Append tangents:
		if (hasTangents)
		{
			std::vector<Float3> m_tangents = mesh->GetTangents();
			mergedTangents.insert(mergedTangents.end(), m_tangents.begin(), m_tangents.end());
		}

		// Append colors:
		if (hasColors)
		{
			std::vector<Float4> colors = mesh->GetColors();
			mergedColors.insert(mergedColors.end(), colors.begin(), colors.end());
		}

		// Append uvs:
		if (HasUVs)
		{
			std::vector<Float4> uvs = mesh->GetUVs();
			mergedUvs.insert(mergedUvs.end(), uvs.begin(), uvs.end());
		}

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
	if (hasNormals)
		mergedMesh->MoveNormals(mergedNormals);
	if (hasTangents)
		mergedMesh->MoveTangents(mergedTangents);
	if (hasColors)
		mergedMesh->MoveColors(mergedColors);
	if (HasUVs)
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
	if (HasNormals())
	{
		str += "\n";
		for (const Float3& normal : m_normals)
			str += normal.ToString() + "\n";
	}
	else
		str += " none\n";

	str += "Tangents:";
	if (HasTangents())
	{
		str += "\n";
		for (const Float3& tangent : m_tangents)
			str += tangent.ToString() + "\n";
	}
	else
		str += " none\n";

	str += "Colors:";
	if (HasColors())
	{
		str += "\n";
		for (const Float4& color : m_colors)
			str += color.ToString() + "\n";
	}
	else
		str += " none\n";

	str += "Uvs:";
	if (HasUVs())
	{
		str += "\n";
		for (const Float4& uv : m_uvs)
			str += uv.ToString() + "\n";
	}
	else
		str += " none\n";

	str += "Triangles:\n";
	for (const Uint3& triangle : m_triangles)
		str += triangle.ToString() + "\n";

	str += "\n";
	return str;
}



// Private:
#ifdef RESIZEABLE_BAR // No staging buffer:
void Mesh::UpdateVertexBuffer(VulkanContext* context)
{
	uint64_t size = static_cast<uint64_t>(GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors() + GetSizeOfUVs());
	vkQueueWaitIdle(context->pLogicalDevice->GetGraphicsQueue().queue);	// wait for previous render calls to finish

	// Resize buffer if necessary:
	if (m_vertexBuffer == nullptr || size != m_vertexBuffer->size)
	{
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_vertexBuffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);
	}

	// Copy positions, colors, uvs:
	void* data;
	VKA(vmaMapMemory(context->GetVmaAllocator(), m_vertexBuffer->allocation, &data));
	memcpy(static_cast<char*>(data), m_positions.data(), GetSizeOfPositions());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions(), m_normals.data(), GetSizeOfNormals());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals(), m_tangents.data(), GetSizeOfTangents());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents(), m_colors.data(), GetSizeOfColors());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors(), m_uvs.data(), GetSizeOfUVs());
	VKA(vmaUnmapMemory(context->GetVmaAllocator(), m_vertexBuffer->allocation));
}
#else // With Staging buffer:
void Mesh::UpdateVertexBuffer(VulkanContext* context)
{
	uint64_t size = static_cast<uint64_t>(GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors() + GetSizeOfUVs());
	vkQueueWaitIdle(context->pLogicalDevice->GetGraphicsQueue().queue);	// wait for previous render calls to finish

	// Resize buffer if necessary:
	if (m_vertexBuffer == nullptr || size != m_vertexBuffer->GetSize())
	{
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_vertexBuffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);
	}

	// Load data into staging buffer:
	std::vector<void*> bufferPointers =
	{ static_cast<void*>(m_positions.data()),
	  static_cast<void*>(m_normals.data()),
	  static_cast<void*>(m_tangents.data()),
	  static_cast<void*>(m_colors.data()),
	  static_cast<void*>(m_uvs.data())};
	std::vector<uint64_t> bufferSizes =
	{ static_cast<uint64_t>(GetSizeOfPositions()),
	  static_cast<uint64_t>(GetSizeOfNormals()),
	  static_cast<uint64_t>(GetSizeOfTangents()),
	  static_cast<uint64_t>(GetSizeOfColors()),
	  static_cast<uint64_t>(GetSizeOfUVs())};
	VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(context, bufferSizes, bufferPointers);

	// Copy data from staging to vertex buffer:
	VmaBuffer::CopyBufferToBuffer(context, &stagingBuffer, m_vertexBuffer.get(), size, context->pLogicalDevice->GetGraphicsQueue());
}
#endif
#ifdef RESIZEABLE_BAR // No staging buffer:
void Mesh::UpdateIndexBuffer(VulkanContext* context)
{
	uint64_t size = static_cast<uint64_t>(GetSizeOfTriangles());
	vkQueueWaitIdle(context->pLogicalDevice->GetGraphicsQueue().queue);	// wait for previous render calls to finish

	// Resize buffer if necessary:
	if (m_indexBuffer == nullptr || size != m_indexBuffer->size)
	{
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_indexBuffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);
	}

	// Copy triangle indexes:
	void* data;
	VKA(vmaMapMemory(context->GetVmaAllocator(), m_indexBuffer->allocation, &data));
	memcpy(data, GetTrianglesUnrolled(), size);
	VKA(vmaUnmapMemory(context->GetVmaAllocator(), m_indexBuffer->allocation));
}
#else // With Staging buffer:
void Mesh::UpdateIndexBuffer(VulkanContext* context)
{
	uint64_t size = static_cast<uint64_t>(GetSizeOfTriangles());
	vkQueueWaitIdle(context->pLogicalDevice->GetGraphicsQueue().queue);	// wait for previous render calls to finish

	// Resize buffer if necessary:
	if (m_indexBuffer == nullptr || size != m_indexBuffer->GetSize())
	{
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_indexBuffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);
	}

	// Load data into staging buffer:
	VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(context, size, GetTrianglesUnrolled());

	// Copy data from staging to vertex buffer:
	VmaBuffer::CopyBufferToBuffer(context, &stagingBuffer, m_indexBuffer.get(), size, context->pLogicalDevice->GetGraphicsQueue());
}
#endif