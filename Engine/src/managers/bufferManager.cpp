#include "bufferManager.h"
#include "uniformBuffer.h"
#include "storageBuffer.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Static members:
	bool BufferManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Buffer>> BufferManager::s_buffers;



	// Initialization and cleanup:
	void BufferManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		StorageBuffer* dummyStorageBuffer = new StorageBuffer(1, 1);
		AddBuffer(dummyStorageBuffer, "dummyStorageBuffer");
	}
	void BufferManager::Clear()
	{
		Context::WaitDeviceIdle();
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
		Context::WaitDeviceIdle();
		s_buffers.erase(name);
	}
}