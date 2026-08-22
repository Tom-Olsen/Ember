#include "vulkanGpuResourceHandle.h"
#include <limits>



namespace vulkanRendererBackend
{
	GpuResourceHandle::GpuResourceHandle()
		: index(std::numeric_limits<uint32_t>::max())
		, generation(0)
	{

	}
	GpuResourceHandle::GpuResourceHandle(uint32_t index, uint64_t generation)
		: index(index), generation(generation)
	{

	}



	bool GpuResourceHandle::IsValid() const
	{
		return index != std::numeric_limits<uint32_t>::max();
	}
	bool GpuResourceHandle::operator==(const GpuResourceHandle& other) const
	{
		return index == other.index && generation == other.generation;
	}
	bool GpuResourceHandle::operator!=(const GpuResourceHandle& other) const
	{
		return !(*this == other);
	}
}