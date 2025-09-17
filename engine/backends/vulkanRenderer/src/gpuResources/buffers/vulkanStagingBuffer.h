#pragma once
#include "vulkanBuffer.h"
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Buffer;
	struct DeviceQueue;
	class VmaImage;



	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= transfer src/dst <para/>
	/// -VmaMemoryUsage				= prefer host <para/>
	/// -VmaAllocationCreateFlags	= mapped, host access random write
	/// </summary>
	class StagingBuffer : public Buffer
	{
	private: // Members:
		void* m_pDeviceData;
		bool isCoherent;
		
	public: // Methods:
		// Constructor/Destructor:
		StagingBuffer(uint64_t size, std::string name);
		~StagingBuffer();

		// Non-copyable:
		StagingBuffer(const StagingBuffer&) = delete;
		StagingBuffer& operator=(const StagingBuffer&) = delete;

		// Movable:
		StagingBuffer(StagingBuffer&&) noexcept = default;
		StagingBuffer& operator=(StagingBuffer&&) noexcept = default;

		// Getters:
		void GetData(void* pDst, uint64_t size, uint64_t offset = 0) const;

		// Setters:
		void SetData(const void* pSrc, uint64_t size, uint64_t offset = 0);

		// Use Texture instead of VmaImage!

		// Upload:
		void UploadToBuffer(VkCommandBuffer commandBuffer, Buffer* pDstBuffer);
		void UploadToBuffer(Buffer* pDstBuffer, const DeviceQueue& queue);
		void UploadToImage(VkCommandBuffer commandBuffer, VmaImage* pDstImage, uint64_t layerCount);
		void UploadToImage(const DeviceQueue& queue, VmaImage* pDstImage, uint64_t layerCount);

		// Download:
		void DownloadFromBuffer(VkCommandBuffer commandBuffer, Buffer* pSrcBuffer);
		void DownloadFromBuffer(Buffer* pSrcBuffer, const DeviceQueue& queue);

		// Not implemented yet, as images are more complex due to layout transitions:
		//void DownloadFromTexture(VkCommandBuffer commandBuffer, VmaImage* pSrcImage);
		//void DownloadFromTexture(Buffer* pSrcBuffer, const DeviceQueue& queue);
	};
}