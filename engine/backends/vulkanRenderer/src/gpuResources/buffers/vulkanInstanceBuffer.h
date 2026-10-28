#pragma once
#include "vulkanStorageBuffer.h"
#include "iInstanceBuffer.h"
#include <cstdint>
#include <memory>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Special double buffer for instancing data.
	/// Buffer copies and history automatically handled by vulkan backend.
	/// </summary>
	class VULKAN_RENDERER_API InstanceBuffer : public StorageBuffer, public emberBackendInterface::IInstanceBuffer
	{
	private: // Members:
		std::unique_ptr<StorageBuffer> m_pHistoryBuffer;
		uint64_t m_initFrameIndex;
		uint64_t m_lastFrameIndex;

	public: // Methods:
		// Constructor/Destructor:
		InstanceBuffer(uint32_t count);
		~InstanceBuffer() override;

		// Non-copyable:
		InstanceBuffer(const InstanceBuffer&) = delete;
		InstanceBuffer& operator=(const InstanceBuffer&) = delete;

		// Non-movable:
		InstanceBuffer(InstanceBuffer&&) = delete;
		InstanceBuffer& operator=(InstanceBuffer&&) = delete;

		// History:
		bool HasHistory() const override;
		emberBackendInterface::IBuffer& GetBufferOld() override;
		void ResetHistory() override;

		// History copy requests:
		void RecordCopyToOldBuffer(emberBackendInterface::ICompute::IQueue& queue) override;

		// Debugging:
		void SetDebugName(const std::string& name) override;
	};
}