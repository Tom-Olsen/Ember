#ifndef __INCLUDE_GUARD_stagingBuffer_h__
#define __INCLUDE_GUARD_stagingBuffer_h__
#include "buffer.h"
#include <string>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class Buffer;
	namespace vulkanBackend
	{
		struct DeviceQueue;
		class VmaImage;
	}

	// EMBER::TODO:
	// - remove/abstract the DeviceQueue dependency, as it relies on vulkanBackend functionality and should not be exposed to the engine side.

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
		std::vector<char> m_hostData;
		
	public: // Methods:
		StagingBuffer(uint64_t size);
		~StagingBuffer();

		// Getters:
		void GetData(void* pDst, uint64_t size) const;
		void GetData(void* pDst, uint64_t size, uint64_t offset) const;

		// Setters:
		void SetData(const void* pSrc, uint64_t size);
		void SetData(const void* pSrc, uint64_t size, uint64_t offset);

		// Upload:
		void UploadToBuffer(VkCommandBuffer commandBuffer, Buffer* pDstBuffer);
		void UploadToBuffer(Buffer* pDstBuffer, const vulkanBackend::DeviceQueue& queue);
		void UploadToImage(VkCommandBuffer commandBuffer, vulkanBackend::VmaImage* pDstImage, uint64_t layerCount);
		void UploadToImage(const vulkanBackend::DeviceQueue& queue, vulkanBackend::VmaImage* pDstImage, uint64_t layerCount);

		// Download:
		void DownloadFromBuffer(VkCommandBuffer commandBuffer, Buffer* pSrcBuffer);
		void DownloadFromBuffer(Buffer* pSrcBuffer, const vulkanBackend::DeviceQueue& queue);
	};
}



#endif // __INCLUDE_GUARD_stagingBuffer_h__