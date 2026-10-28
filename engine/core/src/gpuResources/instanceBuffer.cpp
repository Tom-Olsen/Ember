#include "instanceBuffer.h"
#include "gpuResourceFactory.h"
#include "iBuffer.h"
#include "iInstanceBuffer.h"
#include "logger.h"
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	InstanceBuffer::InstanceBuffer(uint32_t count, const std::string& name)
		: Buffer(GpuResourceFactory::CreateInstanceBuffer(count), name)
	{

	}
	InstanceBuffer::~InstanceBuffer()
	{
		
	}



	// Movable:
	InstanceBuffer::InstanceBuffer(InstanceBuffer&& other) noexcept = default;
	InstanceBuffer& InstanceBuffer::operator=(InstanceBuffer&& other) noexcept = default;



	// History:
	bool InstanceBuffer::HasHistory() const
	{
		if (!IsValid())
		{
			LOG_WARN("InstanceBuffer::HasHistory() failed, buffer invalid.");
			return false;
		}
		return dynamic_cast<emberBackendInterface::IInstanceBuffer&>(*m_pIBuffer).HasHistory();
	}
	void InstanceBuffer::ResetHistory()
	{
		if (!IsValid())
		{
			LOG_WARN("InstanceBuffer::ResetHistory() failed, buffer invalid.");
			return;
		}
		dynamic_cast<emberBackendInterface::IInstanceBuffer&>(*m_pIBuffer).ResetHistory();
	}
}