#include "vulkanPostRenderCompute.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanPoolManager.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanDescriptorSetBinding.h"
#include <filesystem>
#include <vulkan/vulkan.h>




namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	PostRender::PostRender()
	{
		std::filesystem::path shaderDir = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		std::filesystem::path shaderPath = shaderDir / "inOut.comp.spv";
		m_pInOutComputeShader = std::make_unique<ComputeShader>("inOut", shaderPath);
	}
	PostRender::~PostRender()
	{
		ResetComputeCalls();
	}



	// Movable:
	PostRender::PostRender(PostRender&& other) noexcept = default;
	PostRender& PostRender::operator=(PostRender&& other) noexcept = default;



	// Workload recording:
	void PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, emberBackendInterface::IDescriptorSetBinding* pIDescriptorSetBinding)
	{
		// Record static compute call.
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return;
		}
		if (!pIDescriptorSetBinding)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIDescriptorSetBinding is nullptr.");
			return;
		}

		// Setup compute call:
		uint32_t width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(0)->GetWidth();
		uint32_t height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(0)->GetHeight();
		Uint3 threadCount{ width, height, 1 };
		ComputeCall computeCall = { threadCount, static_cast<ComputeShader*>(pIComputeShader), static_cast<DescriptorSetBinding*>(pIDescriptorSetBinding), false, AccessMasks::None::none, AccessMasks::None::none };
		m_computeCalls.push_back(computeCall);
	}
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader)
	{
		// Record dynamic compute call.
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}

		// Setup compute call:
		uint32_t width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(0)->GetWidth();
		uint32_t height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(0)->GetHeight();
		Uint3 threadCount{ width, height, 1 };
		DescriptorSetBinding* pDescriptorSetBinding = PoolManager::CheckOutCallDescriptorSetBinding(static_cast<Shader*>(static_cast<ComputeShader*>(pIComputeShader)));
		ComputeCall computeCall = { threadCount, static_cast<ComputeShader*>(pIComputeShader), pDescriptorSetBinding, true, AccessMasks::None::none, AccessMasks::None::none };
		m_computeCalls.push_back(computeCall);
		return pDescriptorSetBinding;
	}



	// Management:
	std::vector<ComputeCall>& PostRender::GetComputeCalls()
	{
		// Add inOut.comp.hlsl if odd number of post render compute calls, as it simply copies the input to the output texture:
		if (m_computeCalls.size() % 2 == 1)
			RecordComputeShader(m_pInOutComputeShader.get());
		return m_computeCalls;
	}
	void PostRender::ResetComputeCalls()
	{
		// Return all owned pDescriptorSetBinding of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : m_computeCalls)
		{
			if (computeCall.ownsDescriptorSetBinding && computeCall.pComputeShader && computeCall.pDescriptorSetBinding)
				PoolManager::ReturnCallDescriptorSetBinding(computeCall.pComputeShader, computeCall.pDescriptorSetBinding);
		}

		// Remove all computeCalls so next frame can start fresh:
		m_computeCalls.clear();
	}
}