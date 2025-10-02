#include "computeShaderManager.h"
#include "computeShader.h"
#include "logger.h"
#include "macros.h"
#include "emberMath.h"
#include <filesystem>



namespace emberEngine
{
	// Static members:
	bool ComputeShaderManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<ComputeShader>> ComputeShaderManager::s_computeShaders;



	// Initialization/Cleanup:
	void ComputeShaderManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		std::filesystem::path directoryPath = (std::filesystem::path(CORE_SHADERS_DIR) / "bin").make_preferred();
		for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (!entry.is_regular_file())
				continue;

			std::filesystem::path filePath = entry.path();
			if (filePath.extension() == ".spv" && filePath.stem().string().ends_with(".comp"))
			{
				// Extract shader name by removing the ".comp.spv" suffix
				std::string name = filePath.stem().string();		// remove '.spv'
				name = name.substr(0, name.size() - 5);	// remove '.comp'

				ComputeShader computeShader(name, filePath);
				AddComputeShader(std::move(computeShader));
			}
		}

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("ComputeShaderManager initialized.");
		#endif
	}
	void ComputeShaderManager::Clear()
	{
		s_computeShaders.clear();
	}



	// Add/Get/Delete:
	void ComputeShaderManager::AddComputeShader(ComputeShader&& computeShader)
	{
		auto newComputeShader = std::make_unique<ComputeShader>(std::move(computeShader));
		if (!s_computeShaders.emplace(newComputeShader->GetName(), std::move(newComputeShader)).second)
			LOG_WARN("ComputeShader with the name: {} already exists in ComputeShaderManager!", newComputeShader->GetName());
	}
	ComputeShader& ComputeShaderManager::GetComputeShader(const std::string& name)
	{
		auto it = s_computeShaders.find(name);
		if (it == s_computeShaders.end())
			throw std::runtime_error("ComputeShader not found: " + name);
		return *(it->second);
	}
	ComputeShader* ComputeShaderManager::TryGetComputeShader(const std::string& name)
	{
		auto it = s_computeShaders.find(name);
		if (it != s_computeShaders.end())
			return it->second.get();
		LOG_WARN("ComputeShader '{}' not found!", name);
		return nullptr;
	}
	void ComputeShaderManager::DeleteComputeShader(const std::string& name)
	{
		s_computeShaders.erase(name);
	}



	// Debugging:
	void ComputeShaderManager::Print()
	{
		LOG_TRACE("ComputeShaderManager content:");
		for (const auto& pair : s_computeShaders)
			LOG_TRACE(pair.first);
	}
}