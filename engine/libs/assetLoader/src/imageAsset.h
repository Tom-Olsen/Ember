#pragma once
#include <cstddef>
#include <vector>



namespace emberAssetLoader
{
	struct ImageAsset
	{
		int width = 0;
		int height = 0;
		int channels = 0;
		std::vector<std::byte> pixels;
	};
}