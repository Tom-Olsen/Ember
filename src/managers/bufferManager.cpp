#include "bufferManager.h"
#include "uniformBuffer.h"
#include "storageBuffer.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Static members:
	bool BufferManager::s_isInitialized = false;
	VulkanContext* BufferManager::s_pContext;
	std::unordered_map<std::string, std::unique_ptr<Buffer>> BufferManager::s_buffers;



	// Initialization and cleanup:
	void BufferManager::Init(VulkanContext* pContext)
	{
		if (s_isInitialized)
			return;

		s_isInitialized = true;
		s_pContext = pContext;

		StorageBuffer* dummyStorageBuffer = new StorageBuffer(s_pContext, 1, 1);
		AddBuffer(dummyStorageBuffer, "dummyStorageBuffer");
	}
	void BufferManager::Clear()
	{
		s_pContext->WaitDeviceIdle();
		s_buffers.clear();
	}



	// Add/Get/Delete:
	void BufferManager::AddBuffer(Buffer* pBuffer, const std::string& name)
	{
		// If buffer already contained in BufferManager, do nothing.
		if (s_buffers.emplace(name, std::unique_ptr<Buffer>(pBuffer)).second == false)
		{
			LOG_WARN("Buffer with the name: {} already exists in BufferManager!", name);
			return;
		}
	}
	Buffer* BufferManager::GetBuffer(const std::string& name)
	{
		auto it = s_buffers.find(name);
		if (it != s_buffers.end())
			return it->second.get();
		LOG_WARN("Buffer '{}' not found!", name);
		return nullptr;
	}
	void BufferManager::DeleteBuffer(const std::string& name)
	{
		s_pContext->WaitDeviceIdle();
		s_buffers.erase(name);
	}
}