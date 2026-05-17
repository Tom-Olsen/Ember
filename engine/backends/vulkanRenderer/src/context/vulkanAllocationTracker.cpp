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
	void AllocationTracker::AddVmaBufferAllocation(VkBuffer buffer, VmaAllocation allocation, const std::string& name)
	{
		if (!allocation)
			return;

		VmaAllocationInfo allocationInfo = {};
		vmaGetAllocationInfo(Context::GetVmaAllocator(), allocation, &allocationInfo);

		BufferAllocationInfo bufferAllocation = {};
		bufferAllocation.buffer = buffer;
		bufferAllocation.memory = allocationInfo.deviceMemory;
		bufferAllocation.name = name;
		bufferAllocation.offset = allocationInfo.offset;
		bufferAllocation.size = allocationInfo.size;

		m_vmaBufferAllocations[allocation] = bufferAllocation;
	}

	void AllocationTracker::AddVmaImageAllocation(VmaAllocation allocation, const std::string& name)
	{
		if (allocation)
			m_vmaImageAllocations[allocation] = name;
	}

	void AllocationTracker::UpdateVmaBufferAllocationName(VmaAllocation allocation, const std::string& name)
	{
		if (!allocation)
			return;

		auto it = m_vmaBufferAllocations.find(allocation);
		if (it != m_vmaBufferAllocations.end())
			it->second.name = name;
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

	void AllocationTracker::DumpVmaBufferAllocations() const
	{
		LOG_INFO("Tracked VmaBufferAllocations (count = {}):", m_vmaBufferAllocations.size());
		for (const auto& [allocation, info] : m_vmaBufferAllocations)
			LOG_INFO(" - VkBuffer {:#x}, VmaAllocation {}, VkDeviceMemory {:#x}, offset {}, size {}, name '{}'", (uint64_t)info.buffer, fmt::ptr(allocation), (uint64_t)info.memory, info.offset, info.size, info.name);
	}
    void AllocationTracker::DumpVmaImageAllocations() const
	{
		LOG_INFO("Tracked VmaImageAllocations (count = {}):", m_vmaImageAllocations.size());
		for (const auto& [allocation, name] : m_vmaImageAllocations)
			LOG_INFO(" - name '{}'", name);
	}


	// Private methods:
	void AllocationTracker::Cleanup()
	{
		#ifdef VALIDATION_LAYERS_ACTIVE

		if (m_vmaBufferAllocations.size() == 0)
			LOG_TRACE("All VmaBufferAllocations have been destroyed.");
		else
		{
			LOG_CRITICAL("Following VmaBufferAllocations have not been destroyed:");
			for (auto& [alloc, info] : m_vmaBufferAllocations)
				LOG_ERROR(info.name);
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