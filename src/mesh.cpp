#include "mesh.h"
#include "macros.h"



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
		LOG_WARN("normals size does not match vertex count! Setting all normals to zero.");
		this->normals.clear();
		this->normals.resize(vertexCount, Float3());
	}
	verticesUpdated = true;
}
void Mesh::SetColors(std::vector<Float4>& colors)
{
	if (colors.size() == vertexCount)
		this->colors = colors;
	else
	{
		LOG_WARN("colors size does not match vertex count! Setting all colors to zero.");
		this->colors.clear();
		this->colors.resize(vertexCount, Float4());
	}
	verticesUpdated = true;
}
void Mesh::SetUVs(std::vector<Float4>& uvs)
{
	if (uvs.size() == vertexCount)
		this->uvs = uvs;
	else
	{
		LOG_WARN("uvs size does not match vertex count! Setting all uvs to zero.");
		this->uvs.clear();
		this->uvs.resize(vertexCount, Float4());
	}
	verticesUpdated = true;
}
void Mesh::SetTriangles(std::vector<Int3>& triangles)
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
		LOG_WARN("normals size does not match vertex count! Setting all normals to zero.");
		this->normals.clear();
		this->normals.resize(vertexCount, Float3());
	}
	verticesUpdated = true;
}
void Mesh::MoveColors(std::vector<Float4>& colors)
{
	if (colors.size() == vertexCount)
		this->colors = std::move(colors);
	else
	{
		LOG_WARN("colors size does not match vertex count! Setting all colors to zero.");
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
		LOG_WARN("uvs size does not match vertex count! Setting all uvs to zero.");
		this->uvs.clear();
		this->uvs.resize(vertexCount, Float4());
	}
	verticesUpdated = true;
}
void Mesh::MoveTriangles(std::vector<Int3>& triangles)
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
std::vector<Float4>& Mesh::GetColors()
{
	return colors;
}
std::vector<Float4>& Mesh::GetUVs()
{
	return uvs;
}
std::vector<Int3>& Mesh::GetTriangles()
{
	return triangles;
}
uint32_t* Mesh::GetTrianglesUnrolled()
{
	return reinterpret_cast<uint32_t*>(triangles.data());
}
uint32_t Mesh::GetSizeOfPositions() const
{
	return vertexCount * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfNormals() const
{
	return vertexCount * sizeof(Float3);
}
uint32_t Mesh::GetSizeOfColors() const
{
	return vertexCount * sizeof(Float4);
}
uint32_t Mesh::GetSizeOfUVs() const
{
	return vertexCount * sizeof(Float4);
}
uint32_t Mesh::GetSizeOfTriangles() const
{
	return triangleCount * sizeof(Int3);
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
Mesh* Mesh::GetCopy(std::string newName)
{
	Mesh* copy = new Mesh(newName);
	copy->SetPositions(positions);
	copy->SetNormals(normals);
	copy->SetColors(colors);
	copy->SetUVs(uvs);
	copy->SetTriangles(triangles);
	return copy;
}



// Mesh transformation:
Mesh* Mesh::Translate(Float3 translation)
{
	for (Float3& position : positions)
		position += translation;
	verticesUpdated = true;
	return this;
}
Mesh* Mesh::Rotate(Float3 eulerAngles)
{
	Float4x4 rotationMatrix = glm::rotate(Float4x4(1.0f), glm::radians(eulerAngles.x), Float3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.y), Float3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.z), Float3(0.0f, 0.0f, 1.0f));

	for (uint32_t i = 0; i < vertexCount; i++)
	{
		Float4 newPosition = rotationMatrix * Float4(positions[i], 1.0f);
		Float4 newNormal = rotationMatrix * Float4(normals[i], 0.0f);
		positions[i] = Float3(newPosition);
		normals[i] = Float3(newNormal);
	}
	verticesUpdated = true;
	return this;
}
Mesh* Mesh::Scale(Float3 scale)
{
	for (Float3& position : positions)
		position *= scale;
	verticesUpdated = true;
	return this;
}



// Public static methods:
std::vector<VkVertexInputBindingDescription> Mesh::GetBindingDescription()
{
	// binding = index of the binding in the array of bindings
	// stride = number of bytes from one entry to the next
	// inputRate = VK_VERTEX_INPUT_RATE_VERTEX : move to the next data entry after each vertex
	//             VK_VERTEX_INPUT_RATE_INSTANCE : move to the next data entry after each instance
	// => x0y0z0, x1y1z1, ..., r0g0b0a0, r1g1b1a1, ...
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	bindingDescriptions.reserve(4);

	// position binding:
	VkVertexInputBindingDescription positionBindingDescription = {};
	positionBindingDescription.binding = 0;
	positionBindingDescription.stride = sizeof(Float3);
	positionBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescriptions.push_back(positionBindingDescription);

	// normal binding:
	VkVertexInputBindingDescription normalBindingDescription = {};
	normalBindingDescription.binding = 1;
	normalBindingDescription.stride = sizeof(Float3);
	normalBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescriptions.push_back(normalBindingDescription);

	// color binding:
	VkVertexInputBindingDescription colorBindingDescription = {};
	colorBindingDescription.binding = 2;
	colorBindingDescription.stride = sizeof(Float4);
	colorBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescriptions.push_back(colorBindingDescription);

	// uv binding:
	VkVertexInputBindingDescription uvBindingDescription = {};
	uvBindingDescription.binding = 3;
	uvBindingDescription.stride = sizeof(Float4);
	uvBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescriptions.push_back(uvBindingDescription);

	return bindingDescriptions;
}
std::vector<VkVertexInputAttributeDescription> Mesh::GetAttributeDescriptions()
{
	// binding = index of the attribute in the array of attributes
	// location = reference to the location directive of the input in the vertex shader
	// format = how to interpret the data
	// offset = data offset
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	// position attribute:
	VkVertexInputAttributeDescription positionAttributeDescription = {};
	positionAttributeDescription.binding = 0;
	positionAttributeDescription.location = 0;
	positionAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttributeDescription.offset = 0;
	attributeDescriptions.push_back(positionAttributeDescription);

	// normal attribute:
	VkVertexInputAttributeDescription normalAttributeDescription = {};
	normalAttributeDescription.binding = 1;
	normalAttributeDescription.location = 1;
	normalAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttributeDescription.offset = 0;
	attributeDescriptions.push_back(normalAttributeDescription);

	// color attribute:
	VkVertexInputAttributeDescription colorAttributeDescription = {};
	colorAttributeDescription.binding = 2;
	colorAttributeDescription.location = 2;
	colorAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	colorAttributeDescription.offset = 0;
	attributeDescriptions.push_back(colorAttributeDescription);

	// uv attribute:
	VkVertexInputAttributeDescription uvAttributeDescription = {};
	uvAttributeDescription.binding = 3;
	uvAttributeDescription.location = 3;
	uvAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	uvAttributeDescription.offset = 0;
	attributeDescriptions.push_back(uvAttributeDescription);

	return attributeDescriptions;
}
VkIndexType Mesh::GetIndexType()
{
	return VK_INDEX_TYPE_UINT32;
}
uint32_t Mesh::GetBindingCount()
{
	return 4;
}
std::vector<VkDeviceSize> Mesh::GetOffsets()
{
	// Edit this when adding more buffers to the mesh.
	// offsets = { position offset, color offset, uv offset, ... }
	std::vector<VkDeviceSize> offsets = { 0, GetSizeOfPositions(), GetSizeOfPositions() + GetSizeOfNormals(), GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfColors()};
	return offsets;
}
Mesh* Mesh::Merge(std::vector<Mesh*>& meshes, std::string name)
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
	std::vector<Float4> mergedColors;
	std::vector<Float4> mergedUvs;
	std::vector<Int3> mergedTriangles;
	mergedPositions.reserve(vertexCount);
	mergedNormals.reserve(vertexCount);
	mergedColors.reserve(vertexCount);
	mergedUvs.reserve(vertexCount);
	mergedTriangles.reserve(triangleCount);

	for (Mesh* mesh : meshes)
	{
		// Get data of current mesh:
		std::vector<Float3> positions = mesh->GetPositions();
		std::vector<Float3> normals = mesh->GetNormals();
		std::vector<Float4> colors = mesh->GetColors();
		std::vector<Float4> uvs = mesh->GetUVs();
		std::vector<Int3> triangles = mesh->GetTriangles();

		// Append positions (must always be present).
		mergedPositions.insert(mergedPositions.end(), positions.begin(), positions.end());

		// Append normals (use zero if not present).
		if (normals.size() == mesh->vertexCount)
			mergedNormals.insert(mergedNormals.end(), normals.begin(), normals.end());
		else
			mergedNormals.insert(mergedNormals.end(), mesh->vertexCount, Float3());

		// Append colors (use zero if not present).
		if (colors.size() == mesh->vertexCount)
			mergedColors.insert(mergedColors.end(), colors.begin(), colors.end());
		else
			mergedColors.insert(mergedColors.end(), mesh->vertexCount, Float4());

		// Append uvs (use zero if not present).
		if (uvs.size() == mesh->vertexCount)
			mergedUvs.insert(mergedUvs.end(), uvs.begin(), uvs.end());
		else
			mergedUvs.insert(mergedUvs.end(), mesh->vertexCount, Float4());

		// Handle triangles:
		for (int i = 0; i < triangles.size(); i++)
		{
			mergedTriangles.push_back(triangles[i] + Int3(vertCount));
			index++;
		}
		vertCount += mesh->vertexCount;
	}

	// Construct mergedMesh:
	Mesh* mergedMesh = new Mesh(name);
	mergedMesh->MovePositions(mergedPositions);
	mergedMesh->MoveNormals(mergedNormals);
	mergedMesh->MoveColors(mergedColors);
	mergedMesh->MoveUVs(mergedUvs);
	mergedMesh->MoveTriangles(mergedTriangles);
	return mergedMesh;
}



// Non-member:
std::string Mesh::ToString()
{
	std::string str = "Mesh:\n";

	str += "Positions:\n";
	for (const Float3& position : positions)
		str += to_string(position) + "\n";

	str += "Normals:\n";
	for (const Float3& normal : normals)
		str += to_string(normal) + "\n";

	str += "Colors:\n";
	for (const Float4& color : colors)
		str += to_string(color) + "\n";

	str += "Triangles:\n";
	for (const Int3& triangle : triangles)
		str += to_string(triangle) + "\n";
	return str;
}



// Private:
#ifdef RESIZEABLE_BAR // No staging buffer:
void Mesh::UpdateVertexBuffer(VulkanContext* context)
{
	uint64_t size = GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfColors() + GetSizeOfUVs();
	vkQueueWaitIdle(context->logicalDevice->graphicsQueue.queue);	// wait for previous render calls to finish

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
	VKA(vmaMapMemory(context->Allocator(), vertexBuffer->allocation, &data));
	memcpy(static_cast<char*>(data), positions.data(), GetSizeOfPositions());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions(), normals.data(), GetSizeOfNormals());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals(), colors.data(), GetSizeOfColors());
	memcpy(static_cast<char*>(data) + GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfColors(), uvs.data(), GetSizeOfUVs());
	VKA(vmaUnmapMemory(context->Allocator(), vertexBuffer->allocation));
}
#else // With Staging buffer:
void Mesh::UpdateVertexBuffer(VulkanContext* context)
{
	uint64_t size = GetSizeOfPositions() + GetSizeOfNormals() + GetSizeOfColors() + GetSizeOfUVs();
	vkQueueWaitIdle(context->logicalDevice->graphicsQueue.queue);	// wait for previous render calls to finish

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
	std::vector<void*> bufferPointers = { static_cast<void*>(positions.data()), static_cast<void*>(normals.data()), static_cast<void*>(colors.data()), static_cast<void*>(uvs.data()) };
	std::vector<uint64_t> bufferSizes = { static_cast<uint64_t>(GetSizeOfPositions()), static_cast<uint64_t>(GetSizeOfNormals()), static_cast<uint64_t>(GetSizeOfColors()), static_cast<uint64_t>(GetSizeOfUVs()) };
	VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(context, bufferSizes, bufferPointers);

	// Copy data from staging to vertex buffer:
	VmaBuffer::CopyBufferToBuffer(context, &stagingBuffer, vertexBuffer.get(), size, context->logicalDevice->graphicsQueue);
}
#endif
#ifdef RESIZEABLE_BAR // No staging buffer:
void Mesh::UpdateIndexBuffer(VulkanContext* context)
{
	uint64_t size = 3 * GetSizeOfTriangles();
	vkQueueWaitIdle(context->logicalDevice->graphicsQueue.queue);	// wait for previous render calls to finish

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
	VKA(vmaMapMemory(context->Allocator(), indexBuffer->allocation, &data));
	memcpy(data, GetTrianglesUnrolled(), size);
	VKA(vmaUnmapMemory(context->Allocator(), indexBuffer->allocation));
}
#else // With Staging buffer:
void Mesh::UpdateIndexBuffer(VulkanContext* context)
{
	uint64_t size = 3 * GetSizeOfTriangles();
	vkQueueWaitIdle(context->logicalDevice->graphicsQueue.queue);	// wait for previous render calls to finish

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
	VmaBuffer::CopyBufferToBuffer(context, &stagingBuffer, indexBuffer.get(), size, context->logicalDevice->graphicsQueue);
}
#endif