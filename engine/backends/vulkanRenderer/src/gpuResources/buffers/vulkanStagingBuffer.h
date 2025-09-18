#pragma once
#include "vulkanBuffer.h"
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Buffer;
	struct DeviceQueue;
	class Texture;



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

		// Upload:
		void UploadToBuffer(VkCommandBuffer commandBuffer, Buffer* pDstBuffer);
		void UploadToBuffer(Buffer* pDstBuffer, const DeviceQueue& queue);
		void UploadToTexture(VkCommandBuffer commandBuffer, Texture* pDstTexture, uint64_t layerCount);
		void UploadToTexture(const DeviceQueue& queue, Texture* pDstTexture, uint64_t layerCount);

		// Download:
		void DownloadFromBuffer(VkCommandBuffer commandBuffer, Buffer* pSrcBuffer);
		void DownloadFromBuffer(Buffer* pSrcBuffer, const DeviceQueue& queue);
		void DownloadFromTexture(VkCommandBuffer commandBuffer, Texture* pSrcTexture);
		void DownloadFromTexture(Texture* pSrcTexture, const DeviceQueue& queue);
	};
}