#pragma once
#include "buffer.h"



namespace emberCore
{
	/// <summary>
	/// Special double buffer for instancing data.
	/// Buffer copies and history automatically handled by vulkan backend.
	/// </summary>
	class EMBER_CORE_API InstanceBuffer : public Buffer
	{
	public: // Methods:
		// Constructor/Destructor:
		InstanceBuffer(uint32_t count, const std::string& name);
		~InstanceBuffer() override;

		// Non-copyable:
		InstanceBuffer(const InstanceBuffer&) = delete;
		InstanceBuffer& operator=(const InstanceBuffer&) = delete;

		// Movable:
		InstanceBuffer(InstanceBuffer&& other) noexcept;
		InstanceBuffer& operator=(InstanceBuffer&& other) noexcept;

		// History:
		bool HasHistory() const;
		void ResetHistory();
	};
}