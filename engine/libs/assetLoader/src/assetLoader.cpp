// needs to be defined before including stb_image.h, but may not be in a header file!
#define STB_IMAGE_IMPLEMENTATION
#include "assetLoader.h"
#include "stb_image.h"



namespace emberAssetLoader
{
    // Loaders:
	Image LoadImageFile(const std::filesystem::path& path, int desiredChannels)
	{
        Image image = {};

		// Load pixelData:
        int width;
        int height = 0;
        int imageChannels = 0;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pPixels = stbi_load(path.string().c_str(), &width, &height, &imageChannels, desiredChannels);
		if (!pPixels)
			throw std::runtime_error("Failed to load image!");

        // Determine actual channel count:
        int usedChannels = (desiredChannels != 0) ? desiredChannels : imageChannels;

        // Copy pixels into vector:
        std::vector<std::byte> pixels(reinterpret_cast<std::byte*>(pPixels), reinterpret_cast<std::byte*>(pPixels + usedChannels * width * height));
        stbi_image_free(pPixels);

        return Image{width, height, usedChannels, std::move(pixels)};
	}
}