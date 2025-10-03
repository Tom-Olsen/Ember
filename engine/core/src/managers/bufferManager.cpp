#include "bufferManager.h"
#include "buffer.h"
#include "logger.h"
#include "macros.h"



namespace emberEngine
{
	// Static members:
	bool BufferManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Buffer>> BufferManager::s_buffers;



	// Initialization/Cleanup:
	void BufferManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("BufferManager initialized.");
		#endif
	}
	void BufferManager::Clear()
	{
		s_buffers.clear();
	}



	// Add/Get/Delete:
	void BufferManager::AddBuffer(Buffer&& buffer)
	{
		auto newBuffer = std::make_unique<Buffer>(std::move(buffer));
		if (!s_buffers.emplace(newBuffer->GetName(), std::move(newBuffer)).second)
			LOG_WARN("Buffer with the name: {} already exists in BufferManager!", newBuffer->GetName());
	}
	Buffer& BufferManager::GetBuffer(const std::string& name)
	{
		auto it = s_buffers.find(name);
		if (it == s_buffers.end())
			throw std::runtime_error("Buffer not found: " + name);
		return *(it->second);
	}
	Buffer* BufferManager::TryGetBuffer(const std::string& name)
	{
		auto it = s_buffers.find(name);
		if (it != s_buffers.end())
			return it->second.get();
		LOG_WARN("Buffer '{}' not found!", name);
		return nullptr;
	}
	void BufferManager::DeleteBuffer(const std::string& name)
	{
		s_buffers.erase(name);
	}



	// Debugging:
	void BufferManager::Print()
	{
		LOG_TRACE("BufferManager content:");
		for (const auto& pair : s_buffers)
			LOG_TRACE(pair.first);
	}
}