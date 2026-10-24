#include "vulkanDeviceQueue.h"
#include "vulkanMacros.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <vector>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	DeviceQueue::DeviceQueue(VkDevice device, uint32_t familyIndex, uint32_t queueIndex)
		: m_device(device), m_timelineSemaphore(VK_NULL_HANDLE), queue(AcquireQueue(familyIndex, queueIndex)), familyIndex(familyIndex)
	{
		VkSemaphoreTypeCreateInfo typeInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
		typeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
		VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		semaphoreInfo.pNext = &typeInfo;
		VKA(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_timelineSemaphore));
	}
	DeviceQueue::~DeviceQueue()
	{
		// LogicalDevice drains all queues before destroying their synchronization objects.
		vkDestroySemaphore(m_device, m_timelineSemaphore, nullptr);
	}



	// Submission:
	uint64_t DeviceQueue::Submit(const VkSubmitInfo2& submitInfo, VkFence fence) const
	{
		// Only one submission at a time:
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_submittedValue == UINT64_MAX)
			throw std::overflow_error("DeviceQueue timeline exhausted.");

		// Preserve caller-provided semaphore signals before appending this queue's timeline signal:
		std::vector<VkSemaphoreSubmitInfo> signals;
		signals.reserve(static_cast<size_t>(submitInfo.signalSemaphoreInfoCount) + 1);
		for (uint32_t index = 0; index < submitInfo.signalSemaphoreInfoCount; index++)
		{
			if (submitInfo.pSignalSemaphoreInfos[index].semaphore == m_timelineSemaphore)
				throw std::invalid_argument("DeviceQueue::Submit manages its own timeline signal.");
			signals.push_back(submitInfo.pSignalSemaphoreInfos[index]);
		}

		// Build timline semaphore signal:
		uint64_t signalValue = m_submittedValue + 1;
		VkSemaphoreSubmitInfo timelineSignal = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		timelineSignal.semaphore = m_timelineSemaphore;
		timelineSignal.value = signalValue;
		timelineSignal.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		signals.push_back(timelineSignal);

		// Submit:
		VkSubmitInfo2 queueSubmitInfo = submitInfo;
		queueSubmitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(signals.size());
		queueSubmitInfo.pSignalSemaphoreInfos = signals.data();
		VKA(vkQueueSubmit2(queue, 1, &queueSubmitInfo, fence));
		m_submittedValue = signalValue;
		return signalValue;
	}



	// Timeline synchronization:
	VkSemaphore DeviceQueue::GetTimelineSemaphore() const
	{
		return m_timelineSemaphore;
	}
	uint64_t DeviceQueue::GetSubmittedValue() const
	{
		// Must wait for possible parallel Submit(...) to complete to avoid race:
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_submittedValue;
	}
	uint64_t DeviceQueue::GetCompletedValue() const
	{
		// Must wait for possible parallel WaitForValue(...) to complete to avoid race:
		std::lock_guard<std::mutex> lock(m_mutex);
		VKA(vkGetSemaphoreCounterValue(m_device, m_timelineSemaphore, &m_completedValue));
		return m_completedValue;
	}
	void DeviceQueue::WaitForValue(uint64_t value) const
	{
		// Wait for semaphore:
		VkSemaphoreWaitInfo waitInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO };
		waitInfo.semaphoreCount = 1;
		waitInfo.pSemaphores = &m_timelineSemaphore;
		waitInfo.pValues = &value;
		VKA(vkWaitSemaphores(m_device, &waitInfo, UINT64_MAX));

		// Guard m_completedValue write to not cause a race with GetCompletedValue():
		std::lock_guard<std::mutex> lock(m_mutex);
		m_completedValue = std::max(m_completedValue, value);
	}



	// Debugging:
	std::string DeviceQueue::ToString() const
	{
		std::stringstream ss;
		ss << "(" << familyIndex << "," << (void*)queue << ")";
		return ss.str();
	}



	// Queue acquisition:
	VkQueue DeviceQueue::AcquireQueue(uint32_t familyIndex, uint32_t queueIndex) const
	{
		VkQueue deviceQueue = VK_NULL_HANDLE;
		vkGetDeviceQueue(m_device, familyIndex, queueIndex, &deviceQueue);
		if (deviceQueue == VK_NULL_HANDLE)
			throw std::runtime_error("DeviceQueue could not acquire the requested queue.");
		return deviceQueue;
	}
}