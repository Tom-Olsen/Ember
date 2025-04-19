#include "managers.h"



namespace emberEngine
{
	// Initialization and cleanup:
	void Managers::Init(uint32_t renderWidth, uint32_t renderHeight)
	{
		RenderPassManager::Init(renderWidth, renderHeight);
		ComputeShaderManager::Init();
		MaterialManager::Init();
		BufferManager::Init();
		TextureManager::Init();
		SamplerManager::Init();
		MeshManager::Init();
		PoolManager::Init();
	}
	void Managers::Clear()
	{
		PoolManager::Clear();
		MeshManager::Clear();
		SamplerManager::Clear();
		TextureManager::Clear();
		BufferManager::Clear();
		MaterialManager::Clear();
		ComputeShaderManager::Clear();
		RenderPassManager::Clear();
	}
}