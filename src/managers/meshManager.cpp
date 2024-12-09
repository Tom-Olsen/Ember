#include "meshManager.h"
#include "vulkanMacros.h"
#include "meshGenerator.h"
#include "vulkanContext.h"
#include "vmaBuffer.h"



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

	Mesh* unitQuad = MeshGenerator::UnitQuad();
	AddMesh(unitQuad->GetName(), unitQuad);

	Mesh* unitCube = MeshGenerator::UnitCube();
	AddMesh(unitCube->GetName(), unitCube);

	Mesh* halfCube = MeshGenerator::HalfCube();
	AddMesh(halfCube->GetName(), halfCube);

	Mesh* cubeSphere = MeshGenerator::CubeSphere(0.5f, 3, "cubeSphere");
	AddMesh(cubeSphere->GetName(), cubeSphere);

	Mesh* disk = MeshGenerator::Disk(0.5f, 16, "disk");
	AddMesh(disk->GetName(), disk);

	Mesh* arcFlatUv = MeshGenerator::ArcFlatUv(0.3f, 0.7f, 135.0f, 16, "arcFlatUv");
	AddMesh(arcFlatUv->GetName(), arcFlatUv);

	Mesh* arcCurvedUv = MeshGenerator::ArcCurvedUv(0.3f, 0.7f, 135.0f, 16, "arcCurvedUv");
	AddMesh(arcCurvedUv->GetName(), arcCurvedUv);

	Mesh* coneSmooth = MeshGenerator::ConeSmooth(0.5f, 1.0f, 16, "coneSmooth");
	AddMesh(coneSmooth->GetName(), coneSmooth);

	Mesh* coneEdgy = MeshGenerator::ConeEdgy(0.5f, 1.0f, 16, "coneEdgy");
	AddMesh(coneEdgy->GetName(), coneEdgy);

	Mesh* zylinderMantleSmooth = MeshGenerator::ZylinderMantleSmooth(0.5f, 1.0f, 16, "zylinderMantleSmooth");
	AddMesh(zylinderMantleSmooth->GetName(), zylinderMantleSmooth);

	Mesh* zylinderMantleEdgy = MeshGenerator::ZylinderMantleEdgy(0.5f, 1.0f, 16, "zylinderMantleEdgy");
	AddMesh(zylinderMantleEdgy->GetName(), zylinderMantleEdgy);

	Mesh* zylinderSmooth = MeshGenerator::ZylinderSmooth(0.5f, 1.0f, 16, "zylinderSmooth");
	AddMesh(zylinderSmooth->GetName(), zylinderSmooth);

	Mesh* zylinderEdgy = MeshGenerator::ZylinderEdgy(0.5f, 1.0f, 16, "zylinderEdgy");
	AddMesh(zylinderEdgy->GetName(), zylinderEdgy);
	
	Mesh* arrowSmooth = MeshGenerator::ArrowSmooth(Float3::forward, 0.8f, 0.1f, 0.2f, 0.2f, 16, "arrowSmooth");
	AddMesh(arrowSmooth->GetName(), arrowSmooth);

	Mesh* arrowEdgy = MeshGenerator::ArrowEdgy(Float3::forward, 0.8f, 0.1f, 0.2f, 0.2f, 16, "arrowEdgy");
	AddMesh(arrowEdgy->GetName(), arrowEdgy);

	Mesh* threeLeg = MeshGenerator::ThreeLeg();
	AddMesh(threeLeg->GetName(), threeLeg);

	Mesh* grid20x20 = MeshGenerator::Grid(20, 20, "grid20x20");
	AddMesh(grid20x20->GetName(), grid20x20);

	Mesh* grid100x100 = MeshGenerator::Grid(100, 100, "grid100x100");
	AddMesh(grid100x100->GetName(), grid100x100);
}
void MeshManager::UnloadAllMeshes()
{
	context->WaitDeviceIdle();
	for (auto& pair : meshes)
		pair.second->Unload();
}
void MeshManager::Clear()
{
	context->WaitDeviceIdle();
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