#pragma once
#include "meshAsset.h"
#include <filesystem>



namespace emberAssetLoader
{
	namespace MeshAssetLoader
	{
		MeshAsset LoadCsv(const std::filesystem::path& path);
	}
}