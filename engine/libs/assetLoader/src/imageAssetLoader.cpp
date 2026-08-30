// needs to be defined before including stb_image.h, but may not be in a header file!
#define STB_IMAGE_IMPLEMENTATION
#include "imageAssetLoader.h"
#include "logger.h"
#include "stb_image.h"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>



namespace emberAssetLoader
{
	ImageAsset ImageAssetLoader::LoadFile(const std::filesystem::path& path, int desiredChannels, bool flipImage)
	{
		// Error handling:
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Error: File does not exist: {}", path.string());
			return ImageAsset{};
		}
		if (!std::filesystem::is_regular_file(path))
		{
			LOG_ERROR("Error: Path is not a regular file: {}", path.string());
			return ImageAsset{};
		}

		// Load pixelData:
        int width = 0;
        int height = 0;
        int imageChannels = 0;
		stbi_set_flip_vertically_on_load(flipImage);
		stbi_uc* pPixels = stbi_load(path.string().c_str(), &width, &height, &imageChannels, desiredChannels);
		if (!pPixels)
			throw std::runtime_error("Failed to load image!");

        // Determine actual channel count:
        int usedChannels = (desiredChannels != 0) ? desiredChannels : imageChannels;

        // Copy pixels into vector:
        std::vector<std::byte> pixels(reinterpret_cast<std::byte*>(pPixels), reinterpret_cast<std::byte*>(pPixels + usedChannels * width * height));
        stbi_image_free(pPixels);

		return ImageAsset{ width, height, usedChannels, std::move(pixels) };
	}
	ImageAsset ImageAssetLoader::LoadCubeFiles(const std::filesystem::path& directoryPath, int desiredChannels, bool flipImage)
	{
		// Error handling:
		if (!std::filesystem::exists(directoryPath))
			throw std::runtime_error("Given directory path '" + directoryPath.string() + "' does not exist");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("Given directory path '" + directoryPath.string() + "' is not a directory.");

		// Cube face names:
		static const std::array<std::string, 6> directions = { "px", "nx", "py", "ny", "pz", "nz" };

		// Collect file paths:
		std::vector<std::filesystem::path> filePaths(6);
		for (uint32_t i = 0; i < 6; i++)
		{
			bool found = false;
			for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
			{
				if (entry.is_regular_file() && entry.path().stem() == directions[i])
				{
					filePaths[i] = entry.path();
					found = true;
					break;
				}
			}
			if (!found)
				throw std::runtime_error("Missing cubemap face: " + directions[i]);
		}

		// Load all 6 images and append pixel data continuously:
		std::vector<std::byte> data;
		int width = 0;
		int height = 0;
		int channels = 0;
		for (size_t i = 0; i < filePaths.size(); i++)
		{
			ImageAsset imageAsset = LoadFile(filePaths[i], desiredChannels, flipImage);
			if (i == 0)
			{
				width = imageAsset.width;
				height = imageAsset.height;
				channels = imageAsset.channels;
			}
			else
			{
				if (imageAsset.width != width || imageAsset.height != height)
					throw std::runtime_error("Cubemap face size mismatch at: " + filePaths[i].string());
				if (imageAsset.channels != channels)
					throw std::runtime_error("Cubemap face channel count mismatch at: " + filePaths[i].string());
			}
			data.insert(data.end(), imageAsset.pixels.begin(), imageAsset.pixels.end());
		}

		return ImageAsset{ width, height, channels, std::move(data) };
	}
}