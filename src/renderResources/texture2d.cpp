// needs to be defined before including stb_image.h, but may not be in the header file!
#define STB_IMAGE_IMPLEMENTATION 
#include "texture2d.h"



// Constructors:
Texture2d::Texture2d(VulkanContext* context, VmaImage* image, std::string name)
{
	this->context = context;
	this->width = static_cast<int>(image->GetWidth());
	this->height = static_cast<int>(image->GetHeight());
	this->channels = 4;
	this->image = std::unique_ptr<VmaImage>(image);
	this->name = name;
}
Texture2d::Texture2d(VulkanContext* context, const std::filesystem::path& filePath, std::string name)
{
	this->context = context;
	this->name = name;

	// Load image:
	// STBI_rgb_alpha = 4 8-bit channels
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* pixels = stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
		throw std::runtime_error("failed to load texture image!");

	// Create staging buffer:
	uint64_t bufferSize = 4 * width * height;
	VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(context, bufferSize, pixels);

	// Define subresource range:
	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;	// mip levels
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;

	CreateImage(subresourceRange, width, height);
	TransitionImageLayout(subresourceRange, stagingBuffer);
	stbi_image_free(pixels);
}



// Destructor:
Texture2d::~Texture2d()
{

}



// Getters:
uint64_t Texture2d::GetWidth()
{
	return image->GetWidth();
}
uint64_t Texture2d::GetHeight()
{
	return image->GetHeight();
}



// Private methods:
void Texture2d::CreateImage(const VkImageSubresourceRange& subresourceRange, uint32_t width, uint32_t height)
{
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = subresourceRange.levelCount;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	allocInfo.flags = 0;
	allocInfo.requiredFlags = 0;
	allocInfo.preferredFlags = 0;

	image = std::make_unique<VmaImage>(context, imageInfo, allocInfo, subresourceRange);
}
void Texture2d::TransitionImageLayout(const VkImageSubresourceRange& subresourceRange, VmaBuffer& stagingBuffer)
{
	// Transition image layout and create mipLevels:
	image->TransitionLayoutUndefinedToTransfer(subresourceRange);
	VmaBuffer::CopyBufferToImage(context, &stagingBuffer, image.get(), context->logicalDevice->transferQueue);
	image->HandoffTransferToGraphicsQueue(subresourceRange);
	image->GenerateMipmaps(subresourceRange.levelCount);
	// old version that does final image transition and handoff between transfer and graphics queue, but no mipmapping:
	//image->TransitionLayoutTransferToShaderRead(subresourceRange);
}