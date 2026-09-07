#pragma once
#include "iCompute.h"
#include "commonRendererCreateInfo.h"
#include "emberMath.h"
#include "vulkanRendererExport.h"
#include <cstdint>
#include <memory>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Async;
	class PreRender;
	class Render;
	class PostRender;
	class SceneColorTexture2dPair;
	class ScreenSpace;



	class VULKAN_RENDERER_API Compute : public emberBackendInterface::ICompute
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::ICompute::IAsync> m_pIAsync;
		std::unique_ptr<emberBackendInterface::ICompute::IPreRender> m_pIPreRender;
		std::unique_ptr<emberBackendInterface::ICompute::IRender> m_pIRender;
		std::unique_ptr<emberBackendInterface::ICompute::IScreenSpace> m_pIScreenSpace;
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
		ScreenSpace* GetScreenSpaceCompute();
		PostRender* GetPostRenderCompute();
		emberBackendInterface::ICompute::IAsync* GetAsyncComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IPreRender* GetPreRenderComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IRender* GetRenderComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IScreenSpace* GetScreenSpaceComputeInterfaceHandle() override;
		emberBackendInterface::ICompute::IPostRender* GetPostRenderComputeInterfaceHandle() override;

		// Frame lifecycle:
		void UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair);
		void CommitFrame(uint32_t frameIndex);
		void RetireFrame(uint32_t frameIndex);
		void RetireAllFrames();
		void DiscardPendingCalls();
	};
}