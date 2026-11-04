#pragma once
#include "commonMeshGeometry.h"
#include "jsonFwd.h"
#include "meshAsset.h"
#include <filesystem>



namespace emberAssetLoader
{
	class MeshAssetLoader
	{
	public: // Methods:
		static MeshAsset Load(const std::filesystem::path& path);
		static emberCommon::MeshGeometry LoadGeometry(const MeshAsset& meshAsset);



	private: // Methods:
		static emberCommon::MeshGeometry LoadCsv(const std::filesystem::path& path);
		static void ValidateRootMembers(const nlohmann::json& json, const std::filesystem::path& path);
	};
}