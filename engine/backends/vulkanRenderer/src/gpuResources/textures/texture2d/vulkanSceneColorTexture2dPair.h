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
	class SceneColorTexture2dPair
	{
	private: // Members:
		std::array<std::vector<std::unique_ptr<RenderTexture2d>>, 2> m_pTextures;
		std::vector<uint32_t> m_currentTextureIndices;
		RenderTexture2d* m_pFinalTexture = nullptr;

	public: // Methods:
		// Constructor/Destructor:
		SceneColorTexture2dPair(uint32_t width, uint32_t height, uint32_t frameCount);
		~SceneColorTexture2dPair();

		// Non-copyable:
		SceneColorTexture2dPair(const SceneColorTexture2dPair&) = delete;
		SceneColorTexture2dPair& operator=(const SceneColorTexture2dPair&) = delete;

		// Movable:
		SceneColorTexture2dPair(SceneColorTexture2dPair&& other) noexcept = default;
		SceneColorTexture2dPair& operator=(SceneColorTexture2dPair&& other) noexcept = default;

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
		void TransitionLayoutOfCurrenForSampling(VkCommandBuffer commandBuffer, uint32_t frameIndex);

	private: // Methods:
		RenderTexture2d* GetTexture(uint32_t frameIndex, uint32_t textureIndex) const;
	};
}