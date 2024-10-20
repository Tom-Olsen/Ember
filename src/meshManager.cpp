#include "meshManager.h"
#include <numbers>



// Static members:
bool MeshManager::isInitialized = false;
VulkanContext* MeshManager::context;
std::unordered_map<std::string, std::unique_ptr<Mesh>> MeshManager::meshes;



// Initialization and cleanup:
void MeshManager::Init(VulkanContext* vulkanContext)
{
	if (isInitialized)
		return;

	isInitialized = true;
	context = vulkanContext;

	Mesh* unitQuad = UnitQuad();
	AddMesh(unitQuad->name, unitQuad);

	Mesh* unitCube = UnitCube();
	AddMesh(unitCube->name, unitCube);
}
void MeshManager::UnloadAllMeshes()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	for (auto& pair : meshes)
		pair.second->Unload();
}
void MeshManager::Clear()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	meshes.clear();
}



// Add/get/delete:
void MeshManager::AddMesh(const std::string name, Mesh* mesh)
{
	// If mesh already contained in MeshManager, do nothing.
    if (meshes.emplace(name, std::unique_ptr<Mesh>(mesh)).second == false)
    {
		LOG_WARN("Mesh with the name: {} already exists in MeshManager!", name);
        return;
    }
}
Mesh* MeshManager::GetMesh(const std::string& name)
{
    auto it = meshes.find(name);
    if (it != meshes.end())
        return it->second.get();
	LOG_WARN("Mesh '{}' not found!", name);
	return nullptr;
}
void MeshManager::DeleteMesh(const std::string& name)
{
    meshes.erase(name);
}



// Debugging:
void MeshManager::PrintAllMeshNames()
{
	LOG_TRACE("Names of all managed meshes:");
	for (const auto& pair : meshes)
		LOG_TRACE(pair.first);
}



// Default mesh constructors:
Mesh* MeshManager::UnitQuad()
{
	Mesh* mesh = new Mesh("UnitQuad");

	std::vector<Float3> positions;
	positions.emplace_back(-0.5f, -0.5f, 0.0f);
	positions.emplace_back(-0.5f, 0.5f, 0.0f);
	positions.emplace_back(0.5f, -0.5f, 0.0f);
	positions.emplace_back(0.5f, 0.5f, 0.0f);
	mesh->MovePositions(positions);

	std::vector<Float4> colors;
	colors.emplace_back(1.0f, 0.0f, 0.0f, 1.0f);
	colors.emplace_back(0.0f, 1.0f, 0.0f, 1.0f);
	colors.emplace_back(0.0f, 0.0f, 1.0f, 1.0f);
	colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
	mesh->MoveColors(colors);

	std::vector<Float4> uvs;
	uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
	uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);
	uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
	uvs.emplace_back(1.0f, 1.0f, 0.0f, 0.0f);
	mesh->MoveUVs(uvs);

	std::vector<Int3> triangles;
	triangles.emplace_back(0, 1, 2);
	triangles.emplace_back(1, 3, 2);
	mesh->MoveTriangles(triangles);

    return mesh;
}
Mesh* MeshManager::UnitCube()
{
	Mesh* unitQuad = GetMesh("UnitQuad");
	if (unitQuad == nullptr)
		unitQuad = UnitQuad();

	std::vector<Mesh*> faces;
	faces.reserve(6);
	for (uint32_t i = 0; i < 6; i++)
		faces.push_back(unitQuad->GetCopy());

	float pi = static_cast<float>(std::numbers::pi);
	float piHalf = 0.5f * pi;
	faces[0]->Rotate(Float3( piHalf,      0,  0))->Translate(Float3( 0.0f,  0.5f,  0.0f));
	faces[1]->Rotate(Float3(-piHalf,      0, pi))->Translate(Float3( 0.0f, -0.5f,  0.0f));
	faces[2]->Rotate(Float3( piHalf, piHalf,  0))->Translate(Float3(-0.5f,  0.0f,  0.0f));
	faces[3]->Rotate(Float3( piHalf,-piHalf,  0))->Translate(Float3( 0.5f,  0.0f,  0.0f));
	faces[4]->Rotate(Float3(      0,      0,  0))->Translate(Float3( 0.0f,  0.0f,  0.5f));
	faces[5]->Rotate(Float3(     pi,      0,  0))->Translate(Float3( 0.0f,  0.0f, -0.5f));

	return Mesh::Merge(faces, "UnitCube");
}