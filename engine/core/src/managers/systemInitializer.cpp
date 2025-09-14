#include "systemInitializer.h"
#include "bufferManager.h"
#include "computeShaderManager.h"
#include "emberRandom.h"
#include "eventSystem.h"
#include "logger.h"
#include "materialManager.h"
#include "meshManager.h"
#include "poolManager.h"
#include "renderPassManager.h"
#include "samplerManager.h"
#include "taskSystem.h"
#include "textureManager.h"
#include "window.h"



namespace emberEngine
{
	// Initialization/Cleanup:
	void SystemInitializer::Init(int windowWidth, int windowHeight)
	{
		emberLogger::Logger::Init();
		math::Random::Init();
		emberTaskSystem::TaskSystem::Init();
		Window::Init(windowWidth, windowHeight);
		EventSystem::Init();
		ComputeShaderManager::Init();
		MaterialManager::Init();
		BufferManager::Init();
		TextureManager::Init();
		SamplerManager::Init();
		MeshManager::Init();
	}
	void SystemInitializer::Clear()
	{
		MeshManager::Clear();
		SamplerManager::Clear();
		TextureManager::Clear();
		BufferManager::Clear();
		MaterialManager::Clear();
		ComputeShaderManager::Clear();
		EventSystem::Clear();
		Window::Clear();
		emberTaskSystem::TaskSystem::Clear();
	}
}