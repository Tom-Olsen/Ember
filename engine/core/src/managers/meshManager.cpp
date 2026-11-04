#include "meshManager.h"
#include "logger.h"
#include "mesh.h"
#include "meshAssetLoader.h"
#include "meshGenerator.h"
#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>



namespace emberCore
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

		try
		{
			Mesh quad = MeshGenerator::Quad();
			quad.SetName("quad");
			AddMesh(std::move(quad));

			Mesh cube = MeshGenerator::Cube();
			cube.SetName("cube");
			AddMesh(std::move(cube));

			Mesh fullScreenRenderQuad = MeshGenerator::FullScreenRenderQuad();
			fullScreenRenderQuad.SetName("fullScreenRenderQuad");
			AddMesh(std::move(fullScreenRenderQuad));

			Mesh unitCubeInverse = MeshGenerator::Cube();
			unitCubeInverse.InvertFaces();
			unitCubeInverse.SetName("unitCubeInverse");
			AddMesh(std::move(unitCubeInverse));

			Mesh halfCube = MeshGenerator::HalfCube();
			halfCube.SetName("halfCube");
			AddMesh(std::move(halfCube));

			Mesh cubeSphere = MeshGenerator::CubeSphere(0.5f, 8, "cubeSphere");
			AddMesh(std::move(cubeSphere));

			Mesh halfCubeSphere = MeshGenerator::HalfCubeSphere(0.5f, 8, "halfCubeSphere");
			AddMesh(std::move(halfCubeSphere));

			Mesh capsule = MeshGenerator::Capsule(0.5f, 3, 8, "capsule");
			AddMesh(std::move(capsule));

			Mesh disk = MeshGenerator::Disk(0.5f, 16, "disk");
			AddMesh(std::move(disk));

			Mesh arcFlatUv = MeshGenerator::ArcFlatUv(0.3f, 0.7f, Degrees(135.0f), 16, "arcFlatUv");
			AddMesh(std::move(arcFlatUv));

			Mesh arcCurvedUv = MeshGenerator::ArcCurvedUv(0.3f, 0.7f, Degrees(135.0f), 16, "arcCurvedUv");
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

			Mesh translateHandleFrame = MeshGenerator::TranslateHandleFrame(0.5f, 5.0f);
			translateHandleFrame.SetName("translateHandleFrame");
			AddMesh(std::move(translateHandleFrame));

			LoadMeshAssets(std::filesystem::path(ENGINE_RESOURCES_DIR) / "meshAssets");
		}
		catch (...)
		{
			Clear();
			throw;
		}
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



	// Asset loading:
	void MeshManager::LoadMeshAssets(const std::filesystem::path& directoryPath)
	{
		// Error handling:
		if (!s_isInitialized)
			throw std::runtime_error("MeshManager::LoadMeshAssets(...) failed. Mesh manager is not initialized.");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("MeshManager::LoadMeshAssets(...) failed. Directory does not exist: " + directoryPath.string());

		// Collect asset paths:
		std::vector<std::filesystem::path> assetPaths;
		for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(directoryPath))
		{
			if (directoryEntry.is_regular_file() && directoryEntry.path().filename().string().ends_with(".meshAsset.json"))
				assetPaths.push_back(directoryEntry.path());
		}
		std::sort(assetPaths.begin(), assetPaths.end());

		// Load and validate mesh assets:
		std::vector<emberAssetLoader::MeshAsset> meshAssets;
		meshAssets.reserve(assetPaths.size());
		std::unordered_set<std::string> meshNames;
		for (const std::filesystem::path& assetPath : assetPaths)
		{
			emberAssetLoader::MeshAsset meshAsset = emberAssetLoader::MeshAssetLoader::Load(assetPath);
			if (s_meshes.contains(meshAsset.meshName) || !meshNames.insert(meshAsset.meshName).second)
				throw std::runtime_error("MeshManager::LoadMeshAssets(...) failed. Mesh name already exists: " + meshAsset.meshName);
			meshAssets.push_back(std::move(meshAsset));
		}

		// Create meshes:
		try
		{
			for (const emberAssetLoader::MeshAsset& meshAsset : meshAssets)
			{
				emberCommon::MeshGeometry geometry = emberAssetLoader::MeshAssetLoader::LoadGeometry(meshAsset);
				Mesh mesh(meshAsset.meshName);
				mesh.MoveGeometry(std::move(geometry));
				AddMesh(std::move(mesh));
			}
		}
		catch (...)
		{
			for (const emberAssetLoader::MeshAsset& meshAsset : meshAssets)
				s_meshes.erase(meshAsset.meshName);
			throw;
		}
	}



	// Add/Get/Delete:
	void MeshManager::AddMesh(Mesh&& mesh)
	{
		auto newMesh = std::make_unique<Mesh>(std::move(mesh));
		std::string name = newMesh->GetName();
		if (!s_meshes.try_emplace(name, std::move(newMesh)).second)
			LOG_WARN("Mesh with the name: {} already exists in MeshManager!", name);
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