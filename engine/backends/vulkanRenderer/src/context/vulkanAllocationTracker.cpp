#include "vulkanAllocationTracker.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanMacros.h"
#include <iostream>



namespace vulkanRendererBackend
{
	// Initialization/Cleanup:
	AllocationTracker::AllocationTracker()
	{

	}
	AllocationTracker::~AllocationTracker()
	{
		Cleanup();
	}


	// Move semantics:
	AllocationTracker::AllocationTracker(AllocationTracker&& other) noexcept
	{
		MoveFrom(other);
	}
	AllocationTracker& AllocationTracker::operator=(AllocationTracker&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methods:
	void AllocationTracker::Init()
	{

	}
	void AllocationTracker::AddVmaBuffer(VmaBuffer* vmaBuffer)
	{
		if (vmaBuffer != nullptr)
			m_pVmaBuffers.insert(vmaBuffer);
	}
	void AllocationTracker::AddVmaImage(VmaImage* vmaImage)
	{
		if (vmaImage != nullptr)
			m_pVmaImages.insert(vmaImage);
	}
	void AllocationTracker::RemoveVmaBuffer(VmaBuffer* vmaBuffer)
	{
		if (vmaBuffer != nullptr)
			m_pVmaBuffers.erase(vmaBuffer);
	}
	void AllocationTracker::RemoveVmaImage(VmaImage* vmaImage)
	{
		if (vmaImage != nullptr)
			m_pVmaImages.erase(vmaImage);
	}



	// Private methods:
	void AllocationTracker::Cleanup()
	{
		//#ifdef VALIDATION_LAYERS_ACTIVE
		//if (m_pVmaBuffers.size() == 0)
		//	LOG_TRACE("All VmaBuffers have been destroyed.");
		//else
		//{
		//	LOG_CRITICAL("Following VmaBuffers have not been destroyed:");
		//	for (VmaBuffer* vmaBuffer : m_pVmaBuffers)
		//		LOG_ERROR(vmaBuffer->GetName());
		//}
		//
		//if (m_pVmaImages.size() == 0)
		//	LOG_TRACE("All VmaImages have been destroyed.");
		//else
		//{
		//	LOG_CRITICAL("Following VmaImages have not been destroyed:");
		//	for (VmaImage* vmaImage : m_pVmaImages)
		//		LOG_ERROR(vmaImage->GetName());
		//}
		//#endif
		#ifdef VALIDATION_LAYERS_ACTIVE
		if (m_pVmaBuffers.size() == 0)
			std::cerr << "All VmaBuffers have been destroyed." << std::endl;
		else
		{
			std::cerr << "Following VmaBuffers have not been destroyed:" << std::endl;
			for (VmaBuffer* vmaBuffer : m_pVmaBuffers)
				std::cerr << vmaBuffer->GetName() << std::endl;
		}

		if (m_pVmaImages.size() == 0)
			std::cerr << "All VmaImages have been destroyed." << std::endl;
		else
		{
			std::cerr << "Following VmaImages have not been destroyed:" << std::endl;
			for (VmaImage* vmaImage : m_pVmaImages)
				std::cerr << "vmaImage->GetName()" << std::endl;
		}
		#endif
	}
	void AllocationTracker::MoveFrom(AllocationTracker& other) noexcept
	{
		m_pVmaBuffers = std::move(other.m_pVmaBuffers);
		m_pVmaImages = std::move(other.m_pVmaImages);

		other.m_pVmaBuffers.clear();
		other.m_pVmaImages.clear();
	}
}