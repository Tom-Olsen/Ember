#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <vector>



// Forward declaration:
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace vulkanRendererBackend
{
	// Forward declaration:
	class RenderTexture2d;



	/// <summary>
	/// Owns the two stable scene-color textures for every frame in flight and tracks
	/// which one currently contains the latest result of the screen-space and post-processing chain.
	/// </summary>
	class SceneColorPair
	{
	private: // Members:
		// Keep explicit physical per-frame textures because m_pFinalTexture must identify the exact finalized image for GUI sampling.
		// Replace these arrays with two perFrameInFlight logical textures once the texture API can reference a fixed physical frame image.
		std::array<std::vector<std::unique_ptr<RenderTexture2d>>, 2> m_pTextures;
		std::vector<uint32_t> m_currentTextureIndices;
		RenderTexture2d* m_pFinalTexture = nullptr;

	public: // Methods:
		// Constructor/Destructor:
		SceneColorPair(uint32_t width, uint32_t height, uint32_t frameCount);
		~SceneColorPair();

		// Non-copyable:
		SceneColorPair(const SceneColorPair&) = delete;
		SceneColorPair& operator=(const SceneColorPair&) = delete;

		// Movable:
		SceneColorPair(SceneColorPair&& other) noexcept = default;
		SceneColorPair& operator=(SceneColorPair&& other) noexcept = default;

		// Frame state:
		void BeginFrame(uint32_t frameIndex);
		void FinalizeFrame(uint32_t frameIndex);
		void Swap(uint32_t frameIndex);

		// Getters:
		uint32_t GetCurrentTextureIndex(uint32_t frameIndex) const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		RenderTexture2d* GetCurrentTexture(uint32_t frameIndex) const;
		RenderTexture2d* GetNextTexture(uint32_t frameIndex) const;
		RenderTexture2d* GetFinalTexture() const;
		RenderTexture2d& GetRenderTargetTexture(uint32_t frameIndex, uint32_t textureIndex);
		const RenderTexture2d& GetRenderTargetTexture(uint32_t frameIndex, uint32_t textureIndex) const;

		// Layout transitions:
		void TransitionLayoutForCompute(VkCommandBuffer commandBuffer, uint32_t frameIndex);
		void TransitionLayoutOfCurrentForSampling(VkCommandBuffer commandBuffer, uint32_t frameIndex);

	private: // Methods:
		RenderTexture2d* GetTexture(uint32_t frameIndex, uint32_t textureIndex) const;
	};
}