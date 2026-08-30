#pragma once
#include "imageAsset.h"
#include <filesystem>



namespace emberAssetLoader
{
	namespace ImageAssetLoader
	{
		// desiredChannels = 0                    : return image as-is (channels = file’s actual number).
		// desiredChannels = 1, imageChannels == 1: copy
		// desiredChannels = 1, imageChannels == 2: copy R, drop A
		// desiredChannels = 1, imageChannels == 3: compute gray = 0.299R + 0.587G + 0.114B
		// desiredChannels = 1, imageChannels == 4: compute gray = 0.299R + 0.587G + 0.114B, drop A
		// desiredChannels = 2, imageChannels == 1: use R, set A=255
		// desiredChannels = 2, imageChannels == 2: copy
		// desiredChannels = 2, imageChannels == 3: copy RG
		// desiredChannels = 2, imageChannels == 4: copy RG
		// desiredChannels = 3, imageChannels == 1: copy R into all: RRR
		// desiredChannels = 3, imageChannels == 2: use R as gray in RGB, drop B
		// desiredChannels = 3, imageChannels == 3: copy
		// desiredChannels = 3, imageChannels == 4: copy RGB, drop A
		// desiredChannels = 4, imageChannels == 1: use R as gray in RGB, set A=255
		// desiredChannels = 4, imageChannels == 2: use R as gray in RGB, set A=B
		// desiredChannels = 4, imageChannels == 3: copy RGB, set A=255
		// desiredChannels = 4, imageChannels == 4: copy
		ImageAsset LoadFile(const std::filesystem::path& path, int desiredChannels = 0, bool flipImage = true);
		ImageAsset LoadCubeFiles(const std::filesystem::path& path, int desiredChannels = 0, bool flipImage = true);
	}
}