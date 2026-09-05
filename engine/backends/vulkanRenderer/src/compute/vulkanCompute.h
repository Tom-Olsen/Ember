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
	class PreRender;
	class Render;
	class PostRender;



	class VULKAN_RENDERER_API Compute : public emberBackendInterface::ICompute
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::ICompute::IAsync> m_pIAsync;
		std::unique_ptr<emberBackendInterface::ICompute::IPreRender> m_pIPreRender;
		std::unique_ptr<emberBackendInterface::ICompute::IRender> m_pIRender;
		std::unique_ptr<emberBackendInterface::ICompute::IPostRender> m_pIPostRender;

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
		PreRender* GetPreRenderCompute();
		Render* GetRenderCompute();
		PostRender* GetPostRenderCompute();
		emberBackendInterface::ICompute::IAsync* GetAsyncComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IPreRender* GetPreRenderComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IRender* GetRenderComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IPostRender* GetPostRenderComputeInterfaceHandle() override;
	};
}