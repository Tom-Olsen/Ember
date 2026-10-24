#pragma once
#include <cstdint>
#include <mutex>
#include <string>
#include <vulkan/vulkan.h>



// Forward declarations:
typedef struct VkQueue_T* VkQueue;



namespace vulkanRendererBackend
{
	struct DeviceQueue
	{
	private: // Members:
		VkDevice m_device;
		VkSemaphore m_timelineSemaphore;
		mutable std::mutex m_mutex;
		mutable uint64_t m_submittedValue = 0;
		mutable uint64_t m_completedValue = 0;

	public: // Members:
		VkQueue const queue;
		uint32_t const familyIndex;

	public: // Methods:
		// Constructor/Destructor:
		DeviceQueue(VkDevice device, uint32_t familyIndex, uint32_t queueIndex);
		~DeviceQueue();

		// Non-copyable:
		DeviceQueue(const DeviceQueue&) = delete;
		DeviceQueue& operator=(const DeviceQueue&) = delete;

		// Non-movable:
		DeviceQueue(DeviceQueue&&) = delete;
		DeviceQueue& operator=(DeviceQueue&&) = delete;

		// Submission:
		uint64_t Submit(const VkSubmitInfo2& submitInfo, VkFence fence = VK_NULL_HANDLE) const;

		// Timeline synchronization:
		VkSemaphore GetTimelineSemaphore() const;
		uint64_t GetSubmittedValue() const;
		uint64_t GetCompletedValue() const;
		void WaitForValue(uint64_t value) const;

		// Debugging:
		std::string ToString() const;

	private: // Methods:
		// Queue acquisition:
		VkQueue AcquireQueue(uint32_t familyIndex, uint32_t queueIndex) const;
	};
}