// needs to be defined before including stb_image.h, but may not be in the header file!
#define STB_IMAGE_IMPLEMENTATION
#include "logger.h"
#include "texture2d.h"
#include "stb_image.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Constructors/Destructor:
	Texture2d::Texture2d()
	{
		m_pContext = nullptr;
		m_width = 0;
		m_height = 0;
		m_channels = 0;
		m_isStorageImage = false;
		m_pImage = nullptr;
		m_name = "";
	}
	Texture2d::Texture2d(VulkanContext* pContext, VmaImage* pImage, const std::string& name)
	{
		m_pContext = pContext;
		m_width = static_cast<int>(pImage->GetWidth());
		m_height = static_cast<int>(pImage->GetHeight());
		m_channels = 4;
		m_isStorageImage = (pImage->GetVkImageCreateInfo()->usage & VK_IMAGE_USAGE_STORAGE_BIT);
		m_pImage = std::unique_ptr<VmaImage>(pImage);
		m_name = name;
	}
	Texture2d::Texture2d(VulkanContext* pContext, const std::filesystem::path& filePath, const std::string& name, VkFormat format, bool isStorageImage)
	{
		m_pContext = pContext;
		m_isStorageImage = isStorageImage;
		m_name = name;

		// Load image:
		// STBI_rgb_alpha = 4 8-bit channels
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pPixels = stbi_load(filePath.string().c_str(), &m_width, &m_height, &m_channels, STBI_rgb_alpha);
		if (!pPixels)
			throw std::runtime_error("Failed to load texture image!");

		// Create staging buffer:
		uint64_t bufferSize = 4 * m_width * m_height;
		VmaBuffer stagingBuffer = VmaBuffer::StagingBuffer(m_pContext, bufferSize, pPixels);

		// Define subresource range:
		VkImageSubresourceRange* pSubresourceRange = new VkImageSubresourceRange();
		pSubresourceRange->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		pSubresourceRange->baseMipLevel = 0;
		pSubresourceRange->levelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;	// mip levels
		pSubresourceRange->baseArrayLayer = 0;
		pSubresourceRange->layerCount = 1;

		m_pImage = std::unique_ptr<VmaImage>(CreateImage(m_pContext, pSubresourceRange, m_width, m_height, format, (VkImageCreateFlagBits)0, m_isStorageImage));
		TransitionImageLayoutWithMipMapping(pSubresourceRange, stagingBuffer);
		stbi_image_free(pPixels);
	}
	Texture2d::~Texture2d()
	{

	}



	// Public methods:
	// Getters:
	uint64_t Texture2d::GetWidth() const
	{
		return static_cast<uint64_t>(m_width);
	}
	uint64_t Texture2d::GetHeight() const
	{
		return static_cast<uint64_t>(m_height);
	}
	uint64_t Texture2d::GetChannels() const
	{
		return static_cast<uint64_t>(m_channels);
	}
	bool Texture2d::GetIsStorageImage() const
	{
		return m_isStorageImage;
	}
	const VmaImage* const Texture2d::GetVmaImage() const
	{
		return m_pImage.get();
	}
	const std::string& Texture2d::GetName() const
	{
		return m_name;
	}

	// Static spezialised Constructors:
	Texture2d* Texture2d::StorageTexture2d(VulkanContext* pContext, int width, int height, VkFormat format, const std::string& name)
	{ 
		// Define subresource range:
		VkImageSubresourceRange* pSubresourceRange = new VkImageSubresourceRange();
		pSubresourceRange->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		pSubresourceRange->baseMipLevel = 0;
		pSubresourceRange->levelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;	// mip levels
		pSubresourceRange->baseArrayLayer = 0;
		pSubresourceRange->layerCount = 1;

		// Create storage image:
		VmaImage* pImage = CreateImage(pContext, pSubresourceRange, width, height, format, (VkImageCreateFlagBits)0, true);

		// Create Texture2d* and return it:
		return new Texture2d(pContext, pImage, "storageTexture8x8");
	}




	// Protected methods:
	VmaImage* Texture2d::CreateImage(VulkanContext* pContext, VkImageSubresourceRange* pSubresourceRange, uint32_t width, uint32_t height, VkFormat format, VkImageCreateFlagBits imageFlags, bool isStorageImage)
	{
		VkImageCreateInfo* pImageInfo = new VkImageCreateInfo();
		pImageInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		pImageInfo->imageType = VK_IMAGE_TYPE_2D;
		pImageInfo->extent.width = width;
		pImageInfo->extent.height = height;
		pImageInfo->extent.depth = 1;
		pImageInfo->mipLevels = pSubresourceRange->levelCount;
		pImageInfo->arrayLayers = pSubresourceRange->layerCount;
		pImageInfo->format = format;
		pImageInfo->tiling = VK_IMAGE_TILING_OPTIMAL;
		pImageInfo->initialLayout = isStorageImage ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_UNDEFINED;
		pImageInfo->usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		pImageInfo->usage |= (isStorageImage ? VK_IMAGE_USAGE_STORAGE_BIT : VK_IMAGE_USAGE_SAMPLED_BIT);
		pImageInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		pImageInfo->samples = VK_SAMPLE_COUNT_1_BIT;
		pImageInfo->flags = imageFlags;

		VmaAllocationCreateInfo* pAllocInfo = new VmaAllocationCreateInfo();
		pAllocInfo->usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		pAllocInfo->flags = 0;
		pAllocInfo->requiredFlags = 0;
		pAllocInfo->preferredFlags = 0;

		return new VmaImage(pContext, pImageInfo, pAllocInfo, pSubresourceRange);
	}
	void Texture2d::TransitionImageLayout(VkImageSubresourceRange* pSubresourceRange, VmaBuffer& stagingBuffer)
	{
		m_pImage->TransitionLayoutUndefinedToTransfer();
		VmaBuffer::CopyBufferToImage(m_pContext, &stagingBuffer, m_pImage.get(), m_pContext->pLogicalDevice->GetTransferQueue(), pSubresourceRange->layerCount);
		m_pImage->TransitionLayoutTransferToShaderRead();
	}
	void Texture2d::TransitionImageLayoutWithMipMapping(VkImageSubresourceRange* pSubresourceRange, VmaBuffer& stagingBuffer)
	{
		m_pImage->TransitionLayoutUndefinedToTransfer();
		VmaBuffer::CopyBufferToImage(m_pContext, &stagingBuffer, m_pImage.get(), m_pContext->pLogicalDevice->GetTransferQueue(), pSubresourceRange->layerCount);
		m_pImage->HandoffTransferToGraphicsQueue();
		m_pImage->GenerateMipmaps(pSubresourceRange->levelCount);
	}
}