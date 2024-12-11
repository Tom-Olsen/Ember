// needs to be defined before including stb_image.h, but may not be in the header file!
#include "textureCube.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include <array>



// Constructor/Destructor:
TextureCube::TextureCube(VulkanContext* pContext, const std::filesystem::path& folderPath, const std::string& name)
{
	m_pContext = pContext;
	m_name = name;

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
	stbi_uc* pFacePixels = new stbi_uc[bufferSize];
	for (int i = 0; i < 6; i++)
	{
		stbi_uc* pPixels = stbi_load(filePaths[i].string().c_str(), &width, &height, &channels, targetChannels);
		if (!pPixels)
		{
			delete[] pFacePixels;
			stbi_image_free(pPixels);
			throw std::runtime_error("Failed to load cube texture face: " + filePaths[i].string());
		}
		memcpy(&pFacePixels[i * targetChannels * width * height], pPixels, targetChannels * width * height);
		stbi_image_free(pPixels);
	}
	
	// Create staging buffer:
	VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(m_pContext, bufferSize, pFacePixels);
	
	// Define subresource range:
	VkImageSubresourceRange* pSubresourceRange = new VkImageSubresourceRange();
	pSubresourceRange->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	pSubresourceRange->baseMipLevel = 0;
	pSubresourceRange->levelCount = 1;	// mipmapping makes no sense for skyboxes
	pSubresourceRange->baseArrayLayer = 0;
	pSubresourceRange->layerCount = 6;
	
	CreateImage(pSubresourceRange, width, height, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
	TransitionImageLayout(pSubresourceRange, stagingBuffer);
	delete[] pFacePixels;
}
TextureCube::~TextureCube()
{

}