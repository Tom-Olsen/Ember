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
#include "vulkanShaderProperties.h"
#include <filesystem>
#include <vulkan/vulkan.h>




namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	PostRender::PostRender()
	{
		m_callIndex = 0;
		std::filesystem::path shaderDir = (std::filesystem::path(VULKAN_LIBRARY_ROOT_PATH) / "src" / "shaders").make_preferred();
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
	void PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, emberBackendInterface::IShaderProperties* pIShaderProperties)
	{// for static compute calls.
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return;
		}
		if (!pIShaderProperties)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIShaderProperties is nullptr.");
			return;
		}

		// Setup compute call:
		uint32_t width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
		uint32_t height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
		Uint3 threadCount{ width, height, 1 };
		ComputeCall computeCall = { m_callIndex, threadCount, static_cast<ComputeShader*>(pIComputeShader), static_cast<ShaderProperties*>(pIShaderProperties), AccessMasks::None::none, AccessMasks::None::none };
		m_staticComputeCalls.push_back(computeCall);
		m_callIndex++;
	}
	emberBackendInterface::IShaderProperties* PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader)
	{// for dynamic compute calls.
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}

		// Setup compute call:
		uint32_t width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
		uint32_t height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
		Uint3 threadCount{ width, height, 1 };
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties(static_cast<Shader*>(static_cast<ComputeShader*>(pIComputeShader)));
		ComputeCall computeCall = { m_callIndex, threadCount, static_cast<ComputeShader*>(pIComputeShader), pShaderProperties, AccessMasks::None::none, AccessMasks::None::none };
		m_dynamicComputeCalls.push_back(computeCall);
		m_callIndex++;
		return pShaderProperties;
	}



	// Management:
	std::vector<ComputeCall*>& PostRender::GetComputeCallPointers()
	{
		size_t count = m_staticComputeCalls.size() + m_dynamicComputeCalls.size();

		// Add inOut.comp.hlsl if odd number of post render compute calls, as it simply copies the input to the output texture:
		if (count % 2 == 1)
		{
			RecordComputeShader(m_pInOutComputeShader.get());
			count++;
		}

		// Populate draw call pointers vector according to callIndex:
		m_computeCallPointers.resize(count);
		for (auto& computeCall : m_staticComputeCalls)
			m_computeCallPointers[computeCall.callIndex] = &computeCall;
		for (auto& computeCall : m_dynamicComputeCalls)
			m_computeCallPointers[computeCall.callIndex] = &computeCall;

		return m_computeCallPointers;
	}
	void PostRender::ResetComputeCalls()
	{
		// Return all pShaderProperties of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : m_dynamicComputeCalls)
			PoolManager::ReturnShaderProperties(computeCall.pComputeShader, computeCall.pShaderProperties);

		// Remove all computeCalls so next frame can start fresh:
		m_staticComputeCalls.clear();
		m_dynamicComputeCalls.clear();
		m_computeCallPointers.clear();
		m_callIndex = 0;
	}
}