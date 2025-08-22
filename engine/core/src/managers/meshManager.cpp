#include "meshManager.h"
#include "logger.h"
#include "macros.h"
#include "mesh.h"
#include "meshGenerator.h"
#include "meshReader.h"
#include "vulkanContext.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Static members:
	bool MeshManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Mesh>> MeshManager::s_meshes;



	// Initialization and cleanup:
	void MeshManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		Mesh* pUnitQuad = MeshGenerator::UnitQuad();
		pUnitQuad->SetName("unitQuad");
		AddMesh(pUnitQuad);
		
		Mesh* pUnitCube = MeshGenerator::UnitCube();
		pUnitCube->SetName("unitCube");
		AddMesh(pUnitCube);
		
		Mesh* pFullScreenRenderQuad = MeshGenerator::FullScreenRenderQuad();
		pFullScreenRenderQuad->SetName("fullScreenRenderQuad");
		AddMesh(pFullScreenRenderQuad);
		
		Mesh* pUnitCubeInverse = MeshGenerator::UnitCube()->InvertFaces();
		pUnitCubeInverse->SetName("unitCubeInverse");
		AddMesh(pUnitCubeInverse);
		
		Mesh* pHalfCube = MeshGenerator::HalfCube();
		pHalfCube->SetName("halfCube");
		AddMesh(pHalfCube);
		
		Mesh* pCubeSphere = MeshGenerator::CubeSphere(0.5f, 3, "cubeSphere");
		AddMesh(pCubeSphere);
		
		Mesh* pDisk = MeshGenerator::Disk(0.5f, 16, "disk");
		AddMesh(pDisk);
		
		Mesh* pArcFlatUv = MeshGenerator::ArcFlatUv(0.3f, 0.7f, 135.0f, 16, "arcFlatUv");
		AddMesh(pArcFlatUv);
		
		Mesh* pArcCurvedUv = MeshGenerator::ArcCurvedUv(0.3f, 0.7f, 135.0f, 16, "arcCurvedUv");
		AddMesh(pArcCurvedUv);
		
		Mesh* pConeSmooth = MeshGenerator::ConeSmooth(0.5f, 1.0f, 16, "coneSmooth");
		AddMesh(pConeSmooth);
		
		Mesh* pConeFlat = MeshGenerator::ConeFlat(0.5f, 1.0f, 16, "coneFlat");
		AddMesh(pConeFlat);
		
		Mesh* pZylinderSmooth = MeshGenerator::ZylinderSmooth(0.5f, 1.0f, 16, "zylinderSmooth");
		AddMesh(pZylinderSmooth);
		
		Mesh* pZylinderFlat = MeshGenerator::ZylinderFlat(0.5f, 1.0f, 16, "zylinderFlat");
		AddMesh(pZylinderFlat);
		
		Mesh* pArrowSmooth = MeshGenerator::ArrowSmooth(Float3::forward, 0.8f, 0.1f, 0.2f, 0.2f, 16, "arrowSmooth");
		AddMesh(pArrowSmooth);
		
		Mesh* pArrowFlat = MeshGenerator::ArrowFlat(Float3::forward, 0.8f, 0.1f, 0.2f, 0.2f, 16, "arrowFlat");
		AddMesh(pArrowFlat);
		
		Mesh* pThreeLeg = MeshGenerator::ThreeLeg();
		pThreeLeg->SetName("threeLeg");
		AddMesh(pThreeLeg);
		
		Mesh* pFourLeg = MeshGenerator::FourLeg();
		pFourLeg->SetName("fourLeg");
		AddMesh(pFourLeg);
		
		Mesh* pCamera = MeshGenerator::Camera();
		pCamera->SetName("camera");
		AddMesh(pCamera);
		
		Mesh* pFrame = MeshGenerator::Frame(0.15f, 2.0f);
		pFrame->SetName("frame");
		AddMesh(pFrame);
		
		std::filesystem::path pathToFile = (std::string)ENGINE_CORE_PATH + "/meshes/Leb9.csv";
		Mesh* pLeb9 = MeshReader::ReadCsvFile(pathToFile);
		pLeb9->SetName("Leb9");
		AddMesh(pLeb9);
		
		#ifdef LOG_INITIALIZATION
		LOG_TRACE("MeshManager initialized.");
		#endif
	}
	void MeshManager::UnloadAllMeshes()
	{
		Context::WaitDeviceIdle();
		for (auto& pair : s_meshes)
			pair.second->Unload();
	}
	void MeshManager::Clear()
	{
		Context::WaitDeviceIdle();
		s_meshes.clear();
	}



	// Add/get/delete:
	void MeshManager::AddMesh(Mesh* pMesh)
	{
		// If mesh already contained in MeshManager, do nothing.
		if (s_meshes.emplace(pMesh->GetName(), std::unique_ptr<Mesh>(pMesh)).second == false)
		{
			LOG_WARN("Mesh with the name: {} already exists in MeshManager!", pMesh->GetName());
			return;
		}
	}
	Mesh* MeshManager::GetMesh(const std::string& name)
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
	void MeshManager::PrintAllMeshNames()
	{
		LOG_TRACE("Names of all managed s_meshes:");
		for (const auto& pair : s_meshes)
			LOG_TRACE(pair.first);
	}
}