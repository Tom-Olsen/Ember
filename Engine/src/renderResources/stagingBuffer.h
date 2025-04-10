#ifndef __INCLUDE_GUARD_stagingBuffer_h__
#define __INCLUDE_GUARD_stagingBuffer_h__
#include "buffer.h"
#include <string>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	struct VulkanQueue;
	class VmaBuffer;
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
		void UploadToBuffer(VmaBuffer* dstBuffer, const VulkanQueue& queue);
		void UploadToImage(VmaImage* dstImage, const VulkanQueue& queue, uint64_t layerCount);

		// Download:
		void DownloadFromBuffer(VmaBuffer* srcBuffer, const VulkanQueue& queue);
	};
}



#endif // __INCLUDE_GUARD_stagingBuffer_h__