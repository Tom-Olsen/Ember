#include "textureCube.h"
#include "logger.h"



// Constructor/Destructor:
TextureCube::TextureCube(VulkanContext* context, const std::filesystem::path& folderPath, std::string name)
{
	this->context = context;
	this->name = name;

	// Check if folder exists:
	if(!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath))
		throw std::runtime_error("Given path '" + folderPath.string() + "' does not exist");

	// Get paths to the 6 images:
	const std::array<std::string, 6> directions = { "px", "nx", "py", "ny", "pz", "nz" };
	std::array<std::filesystem::path, 6> filePaths;
	for (uint32_t i = 0; i < 6; i ++)
		for (const auto& entry : std::filesystem::directory_iterator(folderPath))
			if (entry.is_regular_file() && entry.path().stem() == directions[i])
				filePaths[i] = entry.path();

	// Get image info:
	int targetChannels = STBI_rgb_alpha;
	int width, height, channels;
	if (!stbi_info(filePaths[0].string().c_str(), &width, &height, &channels))
		throw std::runtime_error("Failed to get image info for: " + filePaths[0].string());

	// Load all 6 faces into continous memory:
	stbi_set_flip_vertically_on_load(false);
	uint64_t bufferSize = 6 * targetChannels * width * height;
	stbi_uc* facePixels = new stbi_uc[bufferSize];
	for (int i = 0; i < 6; i++)
	{
		stbi_uc* pixels = stbi_load(filePaths[i].string().c_str(), &width, &height, &channels, targetChannels);
		if (!pixels)
		{
			delete[] facePixels;
			stbi_image_free(pixels);
			throw std::runtime_error("Failed to load cube texture face: " + filePaths[i].string());
		}
		memcpy(&facePixels[i * targetChannels * width * height], pixels, targetChannels * width * height);
		stbi_image_free(pixels);
	}
	
	// Create staging buffer:
	VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(context, bufferSize, facePixels);
	
	// Define subresource range:
	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;	// mipmapping makes no sense for skyboxes
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 6;
	
	CreateImage(subresourceRange, width, height, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
	TransitionImageLayout(subresourceRange, stagingBuffer);
	delete[] facePixels;
}
TextureCube::~TextureCube()
{

}