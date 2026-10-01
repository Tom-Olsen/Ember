#include "core.h"
#include "bufferManager.h"
#include "compute.h"
#include "computeShaderManager.h"
#include "defaultGpuResources.h"
#include "editor.h"
#include "emberMath.h"
#include "eventSystem.h"
#include "gizmo.h"
#include "gpuBackend.h"
#include "gpuResourceFactory.h"
#include "gpuSort.h"
#include "gui.h"
#include "iGui.h"
#include "iRenderer.h"
#include "iWindow.h"
#include "materialManager.h"
#include "meshManager.h"
#include "renderer.h"
#include "textureManager.h"
#include "window.h"



namespace emberCore
{
	// Public methods:
	// Initialization/Cleanup:
	void Core::Init(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IGpuBackend* pIGpuBackend, emberBackendInterface::IGui* pIGui)
	{
		math::Random::Init();
		InitBackends(pIWindow, pIGpuBackend, pIGui);
		InitManagers();
		InitOther();
	}
	void Core::Clear()
	{
		Renderer::WaitDeviceIdle();
		ClearOther();
		ClearManagers();
		ClearBackends();
		math::Random::Clear();
	}



	// Private methods:
	// Initialization:
	void Core::InitBackends(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IGpuBackend* pIGpuBackend, emberBackendInterface::IGui* pIGui)
	{
		GpuBackend::Init(pIGpuBackend);
		emberBackendInterface::IRenderer* pIRenderer = GpuBackend::GetRendererInterface();
		emberBackendInterface::ICompute* pICompute = GpuBackend::GetComputeInterface();

		// Link backends together:
		pIRenderer->LinkIGuiHandle(pIGui);			// needed so renderer can inject gui draw calls in present renderpass.
		pIRenderer->LinkIComputeHandle(pICompute);	// needed for pre- and post-render compute shaders.
		pIWindow->LinkIGuiHandle(pIGui);			// needed for window->gui event passthrough.
		pIGui->SetEditorCallbacks(Editor::Render, Editor::GetFocusedWindowWantCaptureEvents, emberCore::Editor::GetHoveredWindowWantCaptureEvents);

		// Backend wrappers:
		Window::Init(pIWindow);
		GpuResourceFactory::Init(GpuBackend::GetGpuResourceFactoryInterface());
		DefaultGpuResources::Init(GpuBackend::GetDefaultGpuResourcesInterface());
		Renderer::Init(pIRenderer);
		MaterialManager::Init();
		ComputeShaderManager::Init();
		Compute::Init(pICompute);
		Gui::Init(pIGui);
	}
	void Core::InitManagers()
	{
		BufferManager::Init();
		TextureManager::Init();
		MeshManager::Init();
		Gizmo::Init();
	}
	void Core::InitOther()
	{
		EventSystem::Init();
		GpuSort<uint32_t>::Init();
		GpuSort<int>::Init();
		GpuSort<float>::Init();
		GpuSort<Float2>::Init();
		GpuSort<Float3>::Init();
	}



	// Cleanup:
	void Core::ClearBackends()
	{
		Gui::Clear();
		Compute::Clear();
		ComputeShaderManager::Clear();
		MaterialManager::Clear();
		GpuResourceFactory::Clear();
		Renderer::Clear();
		DefaultGpuResources::Clear();
		GpuBackend::Clear();
		Window::Clear();
	}
	void Core::ClearManagers()
	{
		Gizmo::Clear();
		MeshManager::Clear();
		TextureManager::Clear();
		BufferManager::Clear();
	}
	void Core::ClearOther()
	{
		GpuSort<Float3>::Clear();
		GpuSort<Float2>::Clear();
		GpuSort<float>::Clear();
		GpuSort<int>::Clear();
		GpuSort<uint32_t>::Clear();
		EventSystem::Clear();
	}
}