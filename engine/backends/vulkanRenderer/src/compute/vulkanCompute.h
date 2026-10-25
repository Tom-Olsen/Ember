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
	class SceneColorPair;



	class VULKAN_RENDERER_API Compute : public emberBackendInterface::ICompute
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::ICompute::IAsync> m_pIAsync;
		std::unique_ptr<ComputeQueue> m_pPreRenderComputeQueue;
		std::unique_ptr<ComputeQueue> m_pPostOutlineComputeQueue;
		std::unique_ptr<ComputeQueue> m_pPostGeometryComputeQueue;
		std::unique_ptr<ComputeQueue> m_pPostLightingComputeQueue;
		std::unique_ptr<ComputeQueue> m_pPostRenderComputeQueue;

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
		ComputeQueue* GetPostOutlineCompute();
		ComputeQueue* GetPostGeometryCompute();
		ComputeQueue* GetPostLightingCompute();
		ComputeQueue* GetPostRenderCompute();
		emberBackendInterface::ICompute::IAsync* GetAsyncComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IQueue* GetPreRenderComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IQueue* GetPostOutlineComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IQueue* GetPostGeometryComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IQueue* GetPostLightingComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IQueue* GetPostRenderComputeInterfaceHandle() override;

		// Frame lifecycle:
		uint32_t UpdateShaderData(uint32_t frameIndex, SceneColorPair& sceneColorPair);
		void CommitFrame(uint32_t frameIndex);
		void RetireFrame(uint32_t frameIndex);
		void RetireAllFrames();
		void DiscardPendingCalls();
	};
}