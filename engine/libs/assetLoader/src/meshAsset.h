#pragma once
#include <filesystem>
#include <string>



namespace emberAssetLoader
{
	struct MeshAsset
	{
	public: // Members:
		std::string meshName;
		std::filesystem::path sourcePath;
	};
}