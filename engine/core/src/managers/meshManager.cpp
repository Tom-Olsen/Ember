#include "meshManager.h"
#include "assetLoader.h"
#include "logger.h"
#include "mesh.h"
#include "meshGenerator.h"



namespace emberEngine
{
	// Static members:
	bool MeshManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Mesh>> MeshManager::s_meshes;



	// Initialization/Cleanup:
	void MeshManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		Mesh unitQuad = MeshGenerator::UnitQuad();
		unitQuad.SetName("unitQuad");
		AddMesh(std::move(unitQuad));
		
		Mesh unitCube = MeshGenerator::UnitCube();
		unitCube.SetName("unitCube");
		AddMesh(std::move(unitCube));
		
		Mesh fullScreenRenderQuad = MeshGenerator::FullScreenRenderQuad();
		fullScreenRenderQuad.SetName("fullScreenRenderQuad");
		AddMesh(std::move(fullScreenRenderQuad));
		
		Mesh unitCubeInverse = MeshGenerator::UnitCube();
		unitCubeInverse.InvertFaces();
		unitCubeInverse.SetName("unitCubeInverse");
		AddMesh(std::move(unitCubeInverse));
		
		Mesh halfCube = MeshGenerator::HalfCube();
		halfCube.SetName("halfCube");
		AddMesh(std::move(halfCube));
		
		Mesh cubeSphere = MeshGenerator::CubeSphere(0.5f, 3, "cubeSphere");
		AddMesh(std::move(cubeSphere));
		
		Mesh disk = MeshGenerator::Disk(0.5f, 16, "disk");
		AddMesh(std::move(disk));
		
		Mesh arcFlatUv = MeshGenerator::ArcFlatUv(0.3f, 0.7f, 135.0f, 16, "arcFlatUv");
		AddMesh(std::move(arcFlatUv));
		
		Mesh arcCurvedUv = MeshGenerator::ArcCurvedUv(0.3f, 0.7f, 135.0f, 16, "arcCurvedUv");
		AddMesh(std::move(arcCurvedUv));
		
		Mesh coneSmooth = MeshGenerator::ConeSmooth(0.5f, 1.0f, 16, "coneSmooth");
		AddMesh(std::move(coneSmooth));
		
		Mesh coneFlat = MeshGenerator::ConeFlat(0.5f, 1.0f, 16, "coneFlat");
		AddMesh(std::move(coneFlat));
		
		Mesh zylinderSmooth = MeshGenerator::ZylinderSmooth(0.5f, 1.0f, 16, "zylinderSmooth");
		AddMesh(std::move(zylinderSmooth));
		
		Mesh zylinderFlat = MeshGenerator::ZylinderFlat(0.5f, 1.0f, 16, "zylinderFlat");
		AddMesh(std::move(zylinderFlat));
		
		Mesh arrowSmooth = MeshGenerator::ArrowSmooth(Float3::forward, 0.8f, 0.1f, 0.2f, 0.2f, 16, "arrowSmooth");
		AddMesh(std::move(arrowSmooth));
		
		Mesh arrowFlat = MeshGenerator::ArrowFlat(Float3::forward, 0.8f, 0.1f, 0.2f, 0.2f, 16, "arrowFlat");
		AddMesh(std::move(arrowFlat));
		
		Mesh threeLeg = MeshGenerator::ThreeLeg();
		threeLeg.SetName("threeLeg");
		AddMesh(std::move(threeLeg));
		
		Mesh fourLeg = MeshGenerator::FourLeg();
		fourLeg.SetName("fourLeg");
		AddMesh(std::move(fourLeg));
		
		Mesh camera = MeshGenerator::Camera();
		camera.SetName("camera");
		AddMesh(std::move(camera));
		
		Mesh frame = MeshGenerator::Frame(0.15f, 2.0f);
		frame.SetName("frame");
		AddMesh(std::move(frame));
		
		std::filesystem::path leb9File = (std::filesystem::path(ENGINE_RESOURCES_DIR) / "meshes/Leb9.csv").make_preferred();
		Mesh leb9("leb9");
		leb9.MoveMeshAsset(emberAssetLoader::LoadMeshCsv(leb9File));
		AddMesh(std::move(leb9));
	}
	//void MeshManager::UnloadAllMeshes()
	//{
	//	Context::WaitDeviceIdle();
	//	for (auto& pair : s_meshes)
	//		pair.second->Unload();
	//}
	void MeshManager::Clear()
	{
		s_meshes.clear();
		s_isInitialized = false;
	}



	// Add/Get/Delete:
	void MeshManager::AddMesh(Mesh&& mesh)
	{
		auto newMesh = std::make_unique<Mesh>(std::move(mesh));
		if (!s_meshes.emplace(newMesh->GetName(), std::move(newMesh)).second)
			LOG_WARN("Mesh with the name: {} already exists in MeshManager!", newMesh->GetName());
	}
	Mesh& MeshManager::GetMesh(const std::string& name)
	{
		auto it = s_meshes.find(name);
		if (it == s_meshes.end())
			throw std::runtime_error("Mesh not found: " + name);
		return *(it->second);
	}
	Mesh* MeshManager::TryGetMesh(const std::string& name)
	{
		auto it = s_meshes.find(name);
		if (it != s_meshes.end())
			return it->second.get();
		LOG_WARN("Mesh '{}' not found!", name);
		return nullptr;
	}
	void MeshManager::DeleteMesh(const std::string& name)
	{
		s_meshes.erase(name);
	}



	// Debugging:
	void MeshManager::Print()
	{
		LOG_TRACE("MeshManager content:");
		for (const auto& pair : s_meshes)
			LOG_TRACE(pair.first);
	}
}