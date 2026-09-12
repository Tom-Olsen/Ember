#pragma once
#include "iCompute.h"
#include "vulkanRendererExport.h"
#include <cstdint>
#include <memory>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Async;
	class ComputeQueue;
	class DepthTexture2d;
	class PostRenderComputeQueue;
	class SceneColorTexture2dPair;
	class ScreenSpaceComputeQueue;



	class VULKAN_RENDERER_API Compute : public emberBackendInterface::ICompute
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::ICompute::IAsync> m_pIAsync;
		std::unique_ptr<ComputeQueue> m_pPreRenderComputeQueue;
		std::unique_ptr<ComputeQueue> m_pMidRenderComputeQueue;
		std::unique_ptr<ScreenSpaceComputeQueue> m_pScreenSpaceComputeQueue;
		std::unique_ptr<PostRenderComputeQueue> m_pPostRenderComputeQueue;

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
		ComputeQueue* GetPreRenderCompute();
		ComputeQueue* GetMidRenderCompute();
		ScreenSpaceComputeQueue* GetScreenSpaceCompute();
		PostRenderComputeQueue* GetPostRenderCompute();
		emberBackendInterface::ICompute::IAsync* GetAsyncComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IQueue* GetPreRenderComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IQueue* GetMidRenderComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IQueue* GetScreenSpaceComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IPostRenderQueue* GetPostRenderComputeInterfaceHandle() override;

		// Frame lifecycle:
		uint32_t UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair, DepthTexture2d& sceneDepth);
		void CommitFrame(uint32_t frameIndex);
		void RetireFrame(uint32_t frameIndex);
		void RetireAllFrames();
		void DiscardPendingCalls();
	};
}