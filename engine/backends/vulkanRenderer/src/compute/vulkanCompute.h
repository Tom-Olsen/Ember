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
	class PostRender;
	class PreRender;



	class VULKAN_RENDERER_API Compute : public emberBackendInterface::ICompute
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::ICompute::IAsync> m_pIAsync;
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
		PostRender* GetPostRenderCompute();
		PreRender* GetPreRenderCompute();
		emberBackendInterface::ICompute::IAsync* GetAsyncComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IPostRender* GetPostRenderComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IPreRender* GetPreRenderComputeInterfaceHandle() override;
	};
}