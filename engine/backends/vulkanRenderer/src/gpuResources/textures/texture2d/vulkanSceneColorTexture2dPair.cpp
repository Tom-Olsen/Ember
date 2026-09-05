#include "vulkanSceneColorTexture2dPair.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanRenderTexture2d.h"
#include <stdexcept>
#include <string>
#include <utility>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	SceneColorTexture2dPair::SceneColorTexture2dPair(uint32_t width, uint32_t height, uint32_t frameCount)
	{
		if (frameCount == 0)
			throw std::invalid_argument("SceneColorTexture2dPair::SceneColorTexture2dPair(...) failed. frameCount must not be zero.");

		for (std::vector<std::unique_ptr<RenderTexture2d>>& pTextures : m_pTextures)
			pTextures.reserve(frameCount);
		m_currentTextureIndices.resize(frameCount, 0);

		for (uint32_t frameIndex = 0; frameIndex < frameCount; frameIndex++)
		{
			for (uint32_t textureIndex = 0; textureIndex < m_pTextures.size(); textureIndex++)
			{
				std::unique_ptr<RenderTexture2d> pTexture = std::make_unique<RenderTexture2d>(deferredRenderingContract::sceneColorFormat, width, height);
				std::string textureName = textureIndex == 0 ? "SceneColorTextureA_Frame" : "SceneColorTextureB_Frame";
				pTexture->SetDebugName(textureName + std::to_string(frameIndex));
				pTexture->GetVmaImage()->TransitionLayout(
					VK_IMAGE_LAYOUT_GENERAL,
					VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
					VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
					AccessMasks::TopOfPipe::none,
					AccessMasks::BottomOfPipe::none);
				m_pTextures[textureIndex].push_back(std::move(pTexture));
			}
		}

		m_pFinalTexture = GetTexture(0, 0);
	}
	SceneColorTexture2dPair::~SceneColorTexture2dPair()
	{

	}



	// Frame state:
	void SceneColorTexture2dPair::BeginFrame(uint32_t frameIndex)
	{
		m_currentTextureIndices[frameIndex] = 0;
	}
	void SceneColorTexture2dPair::FinalizeFrame(uint32_t frameIndex)
	{
		m_pFinalTexture = GetCurrentTexture(frameIndex);
	}
	void SceneColorTexture2dPair::Swap(uint32_t frameIndex)
	{
		m_currentTextureIndices[frameIndex] = 1 - m_currentTextureIndices[frameIndex];
	}



	// Getters:
	uint32_t SceneColorTexture2dPair::GetWidth() const
	{
		return m_pTextures[0][0]->GetWidth();
	}
	uint32_t SceneColorTexture2dPair::GetHeight() const
	{
		return m_pTextures[0][0]->GetHeight();
	}
	RenderTexture2d* SceneColorTexture2dPair::GetCurrentTexture(uint32_t frameIndex) const
	{
		return GetTexture(frameIndex, m_currentTextureIndices[frameIndex]);
	}
	RenderTexture2d* SceneColorTexture2dPair::GetNextTexture(uint32_t frameIndex) const
	{
		return GetTexture(frameIndex, 1 - m_currentTextureIndices[frameIndex]);
	}
	RenderTexture2d* SceneColorTexture2dPair::GetFinalTexture() const
	{
		return m_pFinalTexture;
	}
	const std::vector<std::unique_ptr<RenderTexture2d>>& SceneColorTexture2dPair::GetRenderTargetTextures() const
	{
		return m_pTextures[0];
	}



	// Layout transitions:
	void SceneColorTexture2dPair::PrepareForPostProcessing(VkCommandBuffer commandBuffer, uint32_t frameIndex)
	{
		for (const std::vector<std::unique_ptr<RenderTexture2d>>& pTextures : m_pTextures)
		{
			VmaImage* pImage = pTextures[frameIndex]->GetVmaImage();
			if (pImage->GetImageLayout() == VK_IMAGE_LAYOUT_GENERAL)
				continue;
			pImage->TransitionLayout(
				commandBuffer,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
				VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
				AccessMasks::FragmentShader::shaderRead,
				AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite);
		}
	}
	void SceneColorTexture2dPair::PrepareCurrentForSampling(VkCommandBuffer commandBuffer, uint32_t frameIndex)
	{
		VmaImage* pImage = GetCurrentTexture(frameIndex)->GetVmaImage();
		pImage->TransitionLayout(
			commandBuffer,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
			AccessMasks::FragmentShader::none,
			AccessMasks::FragmentShader::sampledRead);
	}



	// Private methods:
	RenderTexture2d* SceneColorTexture2dPair::GetTexture(uint32_t frameIndex, uint32_t textureIndex) const
	{
		if (textureIndex >= m_pTextures.size())
			throw std::out_of_range("SceneColorTexture2dPair::GetTexture(...) failed. Texture index out of range.");
		return m_pTextures[textureIndex][frameIndex].get();
	}
}