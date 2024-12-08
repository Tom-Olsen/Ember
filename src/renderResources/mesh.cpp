#include "mesh.h"
#include "logger.h"



// Constructor:
Mesh::Mesh(std::string name)
{
	this->name = name;
}



// Destructor:
Mesh::~Mesh()
{

}



// Public methods:
void Mesh::Load(VulkanContext* context)
{
	UpdateVertexBuffer(context);
	UpdateIndexBuffer(context);
	verticesUpdated = false;
	indicesUpdated = false;
	isLoaded = true;
}
void Mesh::Unload()
{
	// call destructors and set ptr to nullptrs:
	vertexBuffer.reset();
	indexBuffer.reset();
	verticesUpdated = false;
	indicesUpdated = false;
	isLoaded = false;
}
 


// Setters:
void Mesh::SetPositions(std::vector<Float3>& positions)
{
	this->vertexCount = static_cast<uint32_t>(positions.size());
	this->positions = positions;
	verticesUpdated = true;
}
void Mesh::SetNormals(std::vector<Float3>& normals)
{
	if (normals.size() == vertexCount)
		this->normals = normals;
	else
	{
		LOG_WARN("Mesh '{}' normals size does not match vertex count! Setting all normals to zero.", name);
		this->normals.clear();
		this->normals.resize(vertexCount, Float3());
	}
	verticesUpdated = true;
}
void Mesh::SetTangents(std::vector<Float3>& tangents)
{
	if (tangents.size() == vertexCount)
		this->tangents = tangents;
	else
	{
		LOG_WARN("Mesh '{}' tangents size does not match vertex count! Setting all tangents to zero.", name);
		this->tangents.clear();
		this->tangents.resize(vertexCount, Float3());
	}
	verticesUpdated = true;
}
void Mesh::SetColors(std::vector<Float4>& colors)
{
	if (colors.size() == vertexCount)
		this->colors = colors;
	else
	{
		LOG_WARN("Mesh '{}' colors size does not match vertex count! Setting all colors to zero.", name);
		this->colors.clear();
		this->colors.resize(vertexCount, Float4());
	}
	verticesUpdated = true;
}
void Mesh::SetUniformColor(const Float4& color)
{
	if (colors.size() == vertexCount)
	{
		for (Float4& col : colors)
			col = color;
	}
	else
	{
		this->colors.clear();
		this->colors.resize(vertexCount, color);
	}
	verticesUpdated = true;
}
void Mesh::SetUVs(std::vector<Float4>& uvs)
{
	if (uvs.size() == vertexCount)
		this->uvs = uvs;
	else
	{
		LOG_WARN("Mesh '{}' uvs size does not match vertex count! Setting all uvs to zero.", name);
		this->uvs.clear();
		this->uvs.resize(vertexCount, Float4());
	}
	verticesUpdated = true;
}
void Mesh::SetTriangles(std::vector<Uint3>& triangles)
{
	this->triangleCount = static_cast<uint32_t>(triangles.size());
	this->triangles = triangles;
	indicesUpdated = true;
}



// Movers:
void Mesh::MovePositions(std::vector<Float3>& positions)
{
	this->vertexCount = static_cast<uint32_t>(positions.size());
	this->positions = std::move(positions);
	verticesUpdated = true;
}
void Mesh::MoveNormals(std::vector<Float3>& normals)
{
	if (normals.size() == vertexCount)
		this->normals = std::move(normals);
	else
	{
		LOG_WARN("Mesh '{}' normals size does not match vertex count! Setting all normals to zero.", name);
		this->normals.clear();
		this->normals.resize(vertexCount, Float3());
	}
	verticesUpdated = true;
}
void Mesh::MoveTangents(std::vector<Float3>& tangents)
{
	if (tangents.size() == vertexCount)
		this->tangents = std::move(tangents);
	else
	{
		LOG_WARN("Mesh '{}' tangents size does not match vertex count! Setting all tangents to zero.", name);
		this->tangents.clear();
		this->tangents.resize(vertexCount, Float3());
	}
	verticesUpdated = true;
}
void Mesh::MoveColors(std::vector<Float4>& colors)
{
	if (colors.size() == vertexCount)
		this->colors = std::move(colors);
	else
	{
		LOG_WARN("Mesh '{}' colors size does not match vertex count! Setting all colors to zero.", name);
		this->colors.clear();
		this->colors.resize(vertexCount, Float4());
	}
	verticesUpdated = true;
}
void Mesh::MoveUVs(std::vector<Float4>& uvs)
{
	if (uvs.size() == vertexCount)
		this->uvs = std::move(uvs);
	else
	{
		LOG_WARN("Mesh '{}' uvs size does not match vertex count! Setting all uvs to zero.", name);
		this->uvs.clear();
		this->uvs.resize(vertexCount, Float4());
	}
	verticesUpdated = true;
}
void Mesh::MoveTriangles(std::vector<Uint3>& triangles)
{
	this->triangleCount = static_cast<uint32_t>(triangles.size());
	this->triangles = std::move(triangles);
	indicesUpdated = true;
}



// Getters:
uint32_t Mesh::GetVertexCount() const
{
	return vertexCount;
}
uint32_t Mesh::GetTriangleCount() const
{
	return triangleCount;
}
std::vector<Float3>& Mesh::GetPositions()
{
	return positions;
}
std::vector<Float3>& Mesh::GetNormals()
{
	return normals;
}
std::vector<Float3>& Mesh::GetTangents()
{
	return tangents;
}
std::vector<Float4>& Mesh::GetColors()
{
	return colors;
}
std::vector<Float4>& Mesh::GetUVs()
{
	return uvs;
}
std::vector<Uint3>& Mesh::GetTriangles()
{
	return triangles;
}
uint32_t* Mesh::GetTrianglesUnrolled()
{
	return reinterpret_cast<uint32_t*>(triangles.data());
}
uint32_t Mesh::GetSizeOfPositions() const
{
	return static_cast<uint32_t>(positions.size()) * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfNormals() const
{
	return static_cast<uint32_t>(normals.size()) * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfTangents() const
{
	return static_cast<uint32_t>(tangents.size()) * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfColors() const
{
	return static_cast<uint32_t>(colors.size()) * sizeof(Float4);
}
uint32_t Mesh::GetSizeOfUVs() const
{
	return static_cast<uint32_t>(uvs.size()) * sizeof(Float4);
}
uint32_t Mesh::GetSizeOfTriangles() const
{
	return static_cast<uint32_t>(triangles.size()) * sizeof(Uint3);
}
uint32_t Mesh::GetBindingCount()
{
	return 5;
}
VkBuffer* Mesh::GetBuffers(VulkanContext* context)
{
	if (!isLoaded)
		Load(context);
	buffers[0] = vertexBuffer->GetVkBuffer();
	buffers[1] = vertexBuffer->GetVkBuffer();
	buffers[2] = vertexBuffer->GetVkBuffer();
	buffers[3] = vertexBuffer->GetVkBuffer();
	buffers[4] = vertexBuffer->GetVkBuffer();
	return buffers;
}
VkDeviceSize* Mesh::GetOffsets()
{
	offsets[0] = 0;
	offsets[1] = GetSizeOfPositions();
	offsets[2] = GetSizeOfPositions() + GetSizeOfNormals();
	offsets[3] = GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents();
	offsets[4] = GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors();
	return offsets;
}
VmaBuffer* Mesh::GetVertexBuffer(VulkanContext* context)
{
	if (!isLoaded)
		Load(context);
	if (verticesUpdated && context != nullptr)
	{
		UpdateVertexBuffer(context);
		verticesUpdated = false;
	}
	return vertexBuffer.get();
}
VmaBuffer* Mesh::GetIndexBuffer(VulkanContext* context)
{
	if (!isLoaded)
		Load(context);
	if (indicesUpdated && context != nullptr)
	{
		UpdateIndexBuffer(context);
		indicesUpdated = false;
	}
	return indexBuffer.get();
}
bool Mesh::IsLoaded()
{
	return isLoaded;
}
bool Mesh::HasNormals()
{
	return normals.size() == vertexCount;
}
bool Mesh::HasTangents()
{
	return tangents.size() == vertexCount;
}
bool Mesh::HasColors()
{
	return colors.size() == vertexCount;
}
bool Mesh::HasUVs()
{
	return uvs.size() == vertexCount;
}
Mesh* Mesh::GetCopy(std::string newName)
{
	Mesh* copy = new Mesh(newName);
	copy->SetPositions(positions);
	if (HasNormals())
		copy->SetNormals(normals);
	if (HasTangents())
		copy->SetTangents(tangents);
	if (HasColors())
		copy->SetColors(colors);
	if (HasUVs())
		copy->SetUVs(uvs);
	copy->SetTriangles(triangles);
	return copy;
}



// Mesh transformation:
Mesh* Mesh::Translate(const Float3& translation)
{
	for (Float3& position : positions)
		position += translation;
	verticesUpdated = true;
	return this;
}
Mesh* Mesh::Rotate(const Float3x3& rotation)
{
	bool hasNormals = HasNormals();
	bool hasTangents = HasTangents();
	for (uint32_t i = 0; i < vertexCount; i++)
	{
		positions[i] = rotation * positions[i];
		if (hasNormals)
			normals[i] = rotation * normals[i];
		if (hasTangents)
			tangents[i] = rotation * tangents[i];
	}
	verticesUpdated = true;
	return this;
}
Mesh* Mesh::Rotate(const Float4x4& rotation)
{
	Float3x3 rotation3x3 = Float3x3(rotation);
	bool hasNormals = HasNormals();
	bool hasTangents = HasTangents();
	for (uint32_t i = 0; i < vertexCount; i++)
	{
		positions[i] = rotation3x3 * positions[i];
		if (hasNormals)
			normals[i] = rotation3x3 * normals[i];
		if (hasTangents)
			tangents[i] = rotation3x3 * tangents[i];
	}
	verticesUpdated = true;
	return this;
}
Mesh* Mesh::Scale(const Float3& scale)
{
	for (Float3& position : positions)
		position *= scale;
	verticesUpdated = true;
	return this;
}
Mesh* Mesh::Scale(float scale)
{
	return Scale(Float3(scale));
}
Mesh* Mesh::Subdivide()
{
	std::vector<Uint3> newTriangles;
	newTriangles.reserve(4 * triangleCount);
	bool hasNormals = HasNormals();
	bool hasTangents = HasTangents();
	bool hasColors = HasColors();
	bool hasUVs = HasUVs();

	// Subdivide each triangle:
	uint32_t newVertexIndex = vertexCount;
	for (uint32_t i = 0; i < triangleCount; i++)
	{
		Uint3 triangle = triangles[i];

		// Add positions:
		Float3 positionA = 0.5f * (positions[triangle[0]] + positions[triangle[1]]);
		Float3 positionB = 0.5f * (positions[triangle[1]] + positions[triangle[2]]);
		Float3 positionC = 0.5f * (positions[triangle[2]] + positions[triangle[0]]);
		positions.push_back(positionA);
		positions.push_back(positionB);
		positions.push_back(positionC);

		// Add normals:
		if (hasNormals)
		{
			Float3 normalA = (normals[triangle[0]] + normals[triangle[1]]).Normalize();
			Float3 normalB = (normals[triangle[1]] + normals[triangle[2]]).Normalize();
			Float3 normalC = (normals[triangle[2]] + normals[triangle[0]]).Normalize();
			normals.push_back(normalA);
			normals.push_back(normalB);
			normals.push_back(normalC);
		}

		// Add tangents:
		if (hasTangents)
		{
			Float3 tangentA = (tangents[triangle[0]] + tangents[triangle[1]]).Normalize();
			Float3 tangentB = (tangents[triangle[1]] + tangents[triangle[2]]).Normalize();
			Float3 tangentC = (tangents[triangle[2]] + tangents[triangle[0]]).Normalize();
			tangents.push_back(tangentA);
			tangents.push_back(tangentB);
			tangents.push_back(tangentC);
		}

		// Add colors:
		if (hasColors)
		{
			Float4 colorA = 0.5f * (colors[triangle[0]] + colors[triangle[1]]);
			Float4 colorB = 0.5f * (colors[triangle[1]] + colors[triangle[2]]);
			Float4 colorC = 0.5f * (colors[triangle[2]] + colors[triangle[0]]);
			colors.push_back(colorA);
			colors.push_back(colorB);
			colors.push_back(colorC);
		}

		// Add Uvs:
		if (hasUVs)
		{
			Float4 uvA = 0.5f * (uvs[triangle[0]] + uvs[triangle[1]]);
			Float4 uvB = 0.5f * (uvs[triangle[1]] + uvs[triangle[2]]);
			Float4 uvC = 0.5f * (uvs[triangle[2]] + uvs[triangle[0]]);
			uvs.push_back(uvA);
			uvs.push_back(uvB);
			uvs.push_back(uvC);
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
	MovePositions(positions);
	if (hasNormals)
		MoveNormals(normals);
	if (hasTangents)
		MoveTangents(tangents);
	if (hasColors)
		MoveColors(colors);
	if (hasUVs)
		MoveUVs(uvs);
	MoveTriangles(newTriangles);
	return this;
}
Mesh* Mesh::Spherify(float factor, float radius)
{
	factor = std::clamp(factor, 0.0f, 1.0f);
	radius = std::max(1e-8f, radius);

	for (uint32_t i = 0; i < vertexCount; i++)
	{
		Float3 sphereNormal = positions[i].Normalize();
		positions[i] = radius * (positions[i] + factor * (sphereNormal - positions[i]));
		if (HasNormals())
			normals[i] = (normals[i] + factor * (sphereNormal - normals[i])).Normalize();
		if (HasTangents())
			tangents[i] = Float3::VectorToPlaneProjection(tangents[i], normals[i]).Normalize();
	}

	// Update mesh data (forces bool updates and logic):
	MovePositions(positions);
	if (HasNormals())
		MoveNormals(normals);
	return this;
}
void Mesh::ComputeNormals()
{
	normals.resize(vertexCount, Float3::zero);
	for (auto& triangle : triangles)
	{
		Float3 p0 = positions[triangle[0]];
		Float3 p1 = positions[triangle[1]];
		Float3 p2 = positions[triangle[2]];
		Float3 edge10 = p1 - p0;
		Float3 edge20 = p2 - p0;
		Float3 normal = Float3::Cross(edge10, edge20).Normalize();

		normals[triangle[0]] += normal;
		normals[triangle[1]] += normal;
		normals[triangle[2]] += normal;
	}
	for (uint32_t i = 0; i < vertexCount; i++)
		normals[i].Normalize();
}
void Mesh::ComputeTangents()
{
	if (!HasNormals())
		ComputeNormals();

	tangents.resize(vertexCount, Float3::zero);
	for (auto& triangle : triangles)
	{
		Float3 p0 = positions[triangle[0]];
		Float3 p1 = positions[triangle[1]];
		Float3 p2 = positions[triangle[2]];
		Float3 edge10 = p1 - p0;
		Float3 edge20 = p2 - p0;
		Float2 uv0 = Float2(uvs[triangle[0]]);
		Float2 uv1 = Float2(uvs[triangle[1]]);
		Float2 uv2 = Float2(uvs[triangle[2]]);
		Float2 deltaUv10 = uv1 - uv0;
		Float2 deltaUv20 = uv2 - uv0;

		float denom = (deltaUv10.x * deltaUv20.y - deltaUv20.x * deltaUv10.y);
		float det = std::abs(denom) < 1e-6f ? 1.0f : 1.0f / denom;
		Float3 tangent = det * (deltaUv20.y * edge10 - deltaUv10.y * edge20);
		tangent.Normalize();

		tangents[triangle[0]] += tangent;
		tangents[triangle[1]] += tangent;
		tangents[triangle[2]] += tangent;
	}
	for (uint32_t i = 0; i < vertexCount; i++)
		tangents[i] = Float3::VectorToPlaneProjection(tangents[i], normals[i]).Normalize();
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
			std::vector<Float3> tangents = mesh->GetTangents();
			mergedTangents.insert(mergedTangents.end(), tangents.begin(), tangents.end());
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
		vertCount += mesh->vertexCount;
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
	for (const Float3& position : positions)
		str += position.ToString() + "\n";

	str += "Normals:";
	if (HasNormals())
	{
		str += "\n";
		for (const Float3& normal : normals)
			str += normal.ToString() + "\n";
	}
	else
		str += " none\n";

	str += "Tangents:";
	if (HasTangents())
	{
		str += "\n";
		for (const Float3& tangent : tangents)
			str += tangent.ToString() + "\n";
	}
	else
		str += " none\n";

	str += "Colors:";
	if (HasColors())
	{
		str += "\n";
		for (const Float4& color : colors)
			str += color.ToString() + "\n";
	}
	else
		str += " none\n";

	str += "Uvs:";
	if (HasUVs())
	{
		str += "\n";
		for (const Float4& uv : uvs)
			str += uv.ToString() + "\n";
	}
	else
		str += " none\n";

	str += "Triangles:\n";
	for (const Uint3& triangle : triangles)
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
	if (vertexBuffer == nullptr || size != vertexBuffer->size)
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

		vertexBuffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);
	}

	// Copy positions, colors, uvs:
	void* data;
	VKA(vmaMapMemory(context->GetVmaAllocator(), vertexBuffer->allocation, &data));
	memcpy(static_cast<char*>(data), positions.data(), GetSizeOfPositions());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions(), normals.data(), GetSizeOfNormals());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals(), tangents.data(), GetSizeOfTangents());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents(), colors.data(), GetSizeOfColors());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors(), uvs.data(), GetSizeOfUVs());
	VKA(vmaUnmapMemory(context->GetVmaAllocator(), vertexBuffer->allocation));
}
#else // With Staging buffer:
void Mesh::UpdateVertexBuffer(VulkanContext* context)
{
	uint64_t size = static_cast<uint64_t>(GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfTangents() + GetSizeOfColors() + GetSizeOfUVs());
	vkQueueWaitIdle(context->pLogicalDevice->GetGraphicsQueue().queue);	// wait for previous render calls to finish

	// Resize buffer if necessary:
	if (vertexBuffer == nullptr || size != vertexBuffer->GetSize())
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

		vertexBuffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);
	}

	// Load data into staging buffer:
	std::vector<void*> bufferPointers =
	{ static_cast<void*>(positions.data()),
	  static_cast<void*>(normals.data()),
	  static_cast<void*>(tangents.data()),
	  static_cast<void*>(colors.data()),
	  static_cast<void*>(uvs.data())};
	std::vector<uint64_t> bufferSizes =
	{ static_cast<uint64_t>(GetSizeOfPositions()),
	  static_cast<uint64_t>(GetSizeOfNormals()),
	  static_cast<uint64_t>(GetSizeOfTangents()),
	  static_cast<uint64_t>(GetSizeOfColors()),
	  static_cast<uint64_t>(GetSizeOfUVs())};
	VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(context, bufferSizes, bufferPointers);

	// Copy data from staging to vertex buffer:
	VmaBuffer::CopyBufferToBuffer(context, &stagingBuffer, vertexBuffer.get(), size, context->pLogicalDevice->GetGraphicsQueue());
}
#endif
#ifdef RESIZEABLE_BAR // No staging buffer:
void Mesh::UpdateIndexBuffer(VulkanContext* context)
{
	uint64_t size = static_cast<uint64_t>(GetSizeOfTriangles());
	vkQueueWaitIdle(context->pLogicalDevice->GetGraphicsQueue().queue);	// wait for previous render calls to finish

	// Resize buffer if necessary:
	if (indexBuffer == nullptr || size != indexBuffer->size)
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

		indexBuffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);
	}

	// Copy triangle indexes:
	void* data;
	VKA(vmaMapMemory(context->GetVmaAllocator(), indexBuffer->allocation, &data));
	memcpy(data, GetTrianglesUnrolled(), size);
	VKA(vmaUnmapMemory(context->GetVmaAllocator(), indexBuffer->allocation));
}
#else // With Staging buffer:
void Mesh::UpdateIndexBuffer(VulkanContext* context)
{
	uint64_t size = static_cast<uint64_t>(GetSizeOfTriangles());
	vkQueueWaitIdle(context->pLogicalDevice->GetGraphicsQueue().queue);	// wait for previous render calls to finish

	// Resize buffer if necessary:
	if (indexBuffer == nullptr || size != indexBuffer->GetSize())
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

		indexBuffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);
	}

	// Load data into staging buffer:
	VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(context, size, GetTrianglesUnrolled());

	// Copy data from staging to vertex buffer:
	VmaBuffer::CopyBufferToBuffer(context, &stagingBuffer, indexBuffer.get(), size, context->pLogicalDevice->GetGraphicsQueue());
}
#endif