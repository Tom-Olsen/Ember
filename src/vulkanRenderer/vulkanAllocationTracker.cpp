#include "vulkanAllocationTracker.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Initialization and cleanup:
	VulkanAllocationTracker::VulkanAllocationTracker()
	{

	}
	VulkanAllocationTracker::~VulkanAllocationTracker()
	{
		#ifdef VALIDATION_LAYERS_ACTIVE
		if (m_pVmaBuffers.size() == 0)
			LOG_TRACE("All VmaBuffers have been destroyed.");
		else
		{
			LOG_CRITICAL("Following VmaBuffers have not been destroyed:");
			for(VmaBuffer* vmaBuffer : m_pVmaBuffers)
				LOG_ERROR(vmaBuffer->GetName());
		}

		if (m_pVmaImages.size() == 0)
			LOG_TRACE("All VmaImages have been destroyed.");
		else
		{
			LOG_CRITICAL("Following VmaImages have not been destroyed:");
			for (VmaImage* vmaImage : m_pVmaImages)
				LOG_ERROR(vmaImage->GetName());
		}
		#endif
	}



	// Public methods:
	void VulkanAllocationTracker::AddVmaBuffer(VmaBuffer* vmaBuffer)
	{
		if (vmaBuffer != nullptr)
			m_pVmaBuffers.insert(vmaBuffer);
	}
	void VulkanAllocationTracker::AddVmaImage(VmaImage* vmaImage)
	{
		if (vmaImage != nullptr)
			m_pVmaImages.insert(vmaImage);
	}
	void VulkanAllocationTracker::RemoveVmaBuffer(VmaBuffer* vmaBuffer)
	{
		if (vmaBuffer != nullptr)
			m_pVmaBuffers.erase(vmaBuffer);
	}
	void VulkanAllocationTracker::RemoveVmaImage(VmaImage* vmaImage)
	{
		if (vmaImage != nullptr)
			m_pVmaImages.erase(vmaImage);
	}
}