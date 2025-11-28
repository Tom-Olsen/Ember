#include "core.h"
#include "bufferManager.h"
#include "compute.h"
#include "computeShaderManager.h"
#include "editor.h"
#include "emberMath.h"
#include "eventSystem.h"
#include "gpuSort.h"
#include "gui.h"
#include "iGui.h"
#include "iRenderer.h"
#include "iWindow.h"
#include "logger.h"
#include "materialManager.h"
#include "meshManager.h"
#include "renderer.h"
#include "textureManager.h"
#include "window.h"



namespace emberEngine
{
	// Initialization:
	void Core::InitBasics()
	{
		emberLogger::Logger::Init();
		math::Random::Init();
	}
	void Core::InitBackends(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IRenderer* pIRenderer, emberBackendInterface::ICompute* pICompute, emberBackendInterface::IGui* pIGui)
	{
		// Link backends together:
		pIRenderer->LinkIGuiHandle(pIGui);			// needed so renderer can inject gui draw calls in present renderpass.
		pIRenderer->LinkIComputeHandle(pICompute);	// needed for pre- and post-render compute shaders.
		pIWindow->LinkIGuiHandle(pIGui);			// needed for window->gui event passthrough.
		pIGui->SetEditorCallbacks(emberEngine::Editor::Render, emberEngine::Editor::GetFocusedWindowWantCaptureEvents);

		// Backend wrappers:
		Window::Init(pIWindow);
		Renderer::Init(pIRenderer);
		Compute::Init(pICompute);
		Gui::Init(pIGui);
	}
	void Core::InitManagers()
	{
		ComputeShaderManager::Init();
		MaterialManager::Init();
		BufferManager::Init();
		TextureManager::Init();
		MeshManager::Init();
	}
	void Core::InitOther()
	{
		EventSystem::Init();
		GpuSort<int>::Init();
		GpuSort<float>::Init();
		GpuSort<Float2>::Init();
	}



	// Cleanup:
	void Core::ClearBasics()
	{
		math::Random::Clear();
		emberLogger::Logger::Clear();
	}
	void Core::ClearBackends()
	{
		Gui::Clear();
		Compute::Clear();
		Renderer::Clear();
		Window::Clear();
	}
	void Core::ClearManagers()
	{
		MeshManager::Clear();
		TextureManager::Clear();
		BufferManager::Clear();
		MaterialManager::Clear();
		ComputeShaderManager::Clear();
	}
	void Core::ClearOther()
	{
		GpuSort<Float2>::Clear();
		GpuSort<float>::Clear();
		GpuSort<int>::Clear();
		EventSystem::Clear();
	}
}