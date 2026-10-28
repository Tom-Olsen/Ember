#pragma once
#include "iCompute.h"



namespace emberBackendInterface
{
	class IBuffer;



	// Additional history contract implemented by instance-data buffers.
	class IInstanceBuffer
	{
	public: // Methods:
		// Constructor/Destructor:
		virtual ~IInstanceBuffer() = default;

		// History:
		virtual bool HasHistory() const = 0;
		virtual IBuffer& GetBufferOld() = 0;
		virtual void ResetHistory() = 0;

		// History copy requests:
		virtual void RecordCopyToOldBuffer(ICompute::IQueue& queue) = 0;
	};
}