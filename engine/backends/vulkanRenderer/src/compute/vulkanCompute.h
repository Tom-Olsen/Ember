#pragma once
#include "iCompute.h"
#include "commonRendererCreateInfo.h"
#include "emberMath.h"
#include "vulkanRendererExport.h"
#include <memory>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Async;
	class Immediate;
	class PostRender;
	class PreRender;



	class VULKAN_RENDERER_API Compute : public emberBackendInterface::ICompute
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::ICompute::IAsync> m_pIAsync;
		std::unique_ptr<emberBackendInterface::ICompute::IImmediate> m_pIImmediate;
		std::unique_ptr<emberBackendInterface::ICompute::IPostRender> m_pIPostRender;
		std::unique_ptr<emberBackendInterface::ICompute::IPreRender> m_pIPreRender;

	public: // Methods:
		// Constructor/Destructor:
		Compute();
		~Compute();

		// Non-copyable:
		Compute(const Compute&) = delete;
		Compute& operator=(const Compute&) = delete;

		// Movable:
		Compute(Compute&& other) noexcept;
		Compute& operator=(Compute&& other) noexcept;

		// Getters:
		Async* GetAsyncCompute();
		Immediate* GetImmediateCompute();
		PostRender* GetPostRenderCompute();
		PreRender* GetPreRenderCompute();
	};
}