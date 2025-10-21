#include "vulkanAllocationTracker.h"
#include "logger.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Public methods:
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



	// Registration:
	void AllocationTracker::AddVmaBufferAllocation(VmaAllocation allocation, const std::string& name)
	{
		if (allocation)
			m_vmaBufferAllocations[allocation] = name;
	}

	void AllocationTracker::AddVmaImageAllocation(VmaAllocation allocation, const std::string& name)
	{
		if (allocation)
			m_vmaImageAllocations[allocation] = name;
	}

	void AllocationTracker::RemoveVmaBufferAllocation(VmaAllocation allocation)
	{
		if (allocation)
			m_vmaBufferAllocations.erase(allocation);
	}

	void AllocationTracker::RemoveVmaImageAllocation(VmaAllocation allocation)
	{
		if (allocation)
			m_vmaImageAllocations.erase(allocation);
	}



	// Private methods:
	void AllocationTracker::Cleanup()
	{
		#ifdef VALIDATION_LAYERS_ACTIVE
		//char* stats = nullptr;
		//vmaBuildStatsString(Context::GetVmaAllocator(), &stats, true);
		//LOG_TRACE(stats);
		//vmaFreeStatsString(Context::GetVmaAllocator(), stats);

		if (m_vmaBufferAllocations.size() == 0)
			LOG_TRACE("All VmaBufferAllocations have been destroyed.");
		else
		{
			LOG_CRITICAL("Following VmaBufferAllocations have not been destroyed:");
			for (auto& [alloc, name] : m_vmaBufferAllocations)
				LOG_ERROR(name);
		}
		
		if (m_vmaImageAllocations.size() == 0)
			LOG_TRACE("All VmaImagesAllocations have been destroyed.");
		else
		{
			LOG_CRITICAL("Following VmaImagesAllocations have not been destroyed:");
			for (auto& [alloc, name] : m_vmaImageAllocations)
				LOG_ERROR(name);
		}
		#endif
	}
	void AllocationTracker::MoveFrom(AllocationTracker& other) noexcept
	{
		m_vmaBufferAllocations = std::move(other.m_vmaBufferAllocations);
		m_vmaImageAllocations = std::move(other.m_vmaImageAllocations);

		other.m_vmaBufferAllocations.clear();
		other.m_vmaImageAllocations.clear();
	}
}