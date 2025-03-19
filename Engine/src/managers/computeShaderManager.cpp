#include "computeShaderManager.h"
#include "computeShader.h"
#include "logger.h"
#include "emberMath.h"
#include "vulkanContext.h"
#include <filesystem>



namespace emberEngine
{
	// Static members:
	bool ComputeShaderManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<ComputeShader>> ComputeShaderManager::s_computeShaders;



	// Initialization and cleanup:
	void ComputeShaderManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		std::filesystem::path directoryPath = (std::string)ENGINE_ROOT_PATH + "/src/shaders/";
		for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_regular_file())
			{
				std::string filename = entry.path().filename().string();
				if (filename.size() > 9 && filename.substr(filename.size() - 9) == ".comp.spv")
				{
					std::string name = filename.substr(0, filename.size() - 9);
					ComputeShader* pComputeShader = new ComputeShader(name, entry.path());
					AddComputeShader(pComputeShader);
				}
			}
		}
	}
	void ComputeShaderManager::Clear()
	{
		VulkanContext::WaitDeviceIdle();
		s_computeShaders.clear();
	}



	// Add/get/delete:
	void ComputeShaderManager::AddComputeShader(ComputeShader* pComputeShader)
	{
		// If compute shader already contained in ComputeShaderManager, do nothing.
		if (s_computeShaders.emplace(pComputeShader->GetName(), std::unique_ptr<ComputeShader>(pComputeShader)).second == false)
		{
			LOG_WARN("ComputeShader with the name: {} already exists in ComputeShaderManager!", pComputeShader->GetName());
			return;
		}
	}
	ComputeShader* ComputeShaderManager::GetComputeShader(const std::string& name)
	{
		auto it = s_computeShaders.find(name);
		if (it != s_computeShaders.end())
			return it->second.get();
		LOG_WARN("ComputeShader '{}' not found!", name);
		return nullptr;
	}
	void ComputeShaderManager::DeleteComputeShader(const std::string& name)
	{
		VulkanContext::WaitDeviceIdle();
		s_computeShaders.erase(name);
	}



	// Debugging:
	void ComputeShaderManager::PrintAllComputeShaderNames()
	{
		LOG_TRACE("Names of all managed compute shaders:");
		for (const auto& pair : s_computeShaders)
			LOG_TRACE(pair.first);
	}
}