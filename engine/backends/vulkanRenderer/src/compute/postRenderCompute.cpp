#include "postRenderCompute.h"
#include "computeCall.h"
#include "computeShader.h"
#include "emberMath.h"
#include "logger.h"
#include "poolManager.h"
#include "renderTexture2d.h"
#include "shaderProperties.h"
#include "vulkanAccessMasks.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanRenderPassManager.h"
#include <filesystem>




namespace vulkanRendererBackend
{
	// Static members:
	bool PostRender::s_isInitialized = false;
	uint32_t PostRender::s_callIndex = 0;
	std::vector<ComputeCall> PostRender::s_staticComputeCalls;
	std::vector<ComputeCall> PostRender::s_dynamicComputeCalls;
	std::vector<ComputeCall*> PostRender::s_computeCallPointers;
	std::unique_ptr<ComputeShader> PostRender::s_pInOutComputeShader;



	// Initialization/Cleanup:
	void PostRender::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		std::filesystem::path shaderDir = std::filesystem::path(VULKAN_LIBRARY_ROOT_PATH) / "src" / "shaders";
		std::filesystem::path shaderPath = shaderDir / "inOut.comp.spv";
		s_pInOutComputeShader = std::make_unique<ComputeShader>("inOut", shaderPath);
	}
	void PostRender::Clear()
	{
		ResetComputeCalls();
	}



	// Workload recording:
	ShaderProperties* PostRender::RecordComputeShader(ComputeShader* pComputeShader)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pComputeShader is nullptr.");
			return nullptr;
		}

		// Setup compute call:
		uint32_t width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
		uint32_t height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
		Uint3 threadCount{ width, height, 1 };
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties(pComputeShader);
		ComputeCall computeCall = { s_callIndex, threadCount, pComputeShader, pShaderProperties, accessMask::none::none, accessMask::none::none };
		s_dynamicComputeCalls.push_back(computeCall);
		s_callIndex++;

		// By returning pShaderProperties, we allow user to change the shader properties of the compute call:
		return pShaderProperties;
	}
	void PostRender::RecordComputeShader(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pComputeShader is nullptr.");
			return;
		}
		if (!pShaderProperties)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pShaderProperties is nullptr.");
			return;
		}

		// Setup compute call:
		uint32_t width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetWidth();
		uint32_t height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture()->GetHeight();
		Uint3 threadCount{ width, height, 1 };
		ComputeCall computeCall = { s_callIndex, threadCount, pComputeShader, pShaderProperties, accessMask::none::none, accessMask::none::none };
		s_staticComputeCalls.push_back(computeCall);
		s_callIndex++;
	}



	// Management:
	std::vector<ComputeCall*>& PostRender::GetComputeCallPointers()
	{
		size_t count = s_staticComputeCalls.size() + s_dynamicComputeCalls.size();

		// Add inOut.comp.hlsl if odd number of post render compute calls, as it simply copies the input to the output texture:
		if (count % 2 == 1)
		{
			RecordComputeShader(s_pInOutComputeShader.get());
			count++;
		}

		// Populate draw call pointers vector according to callIndex:
		s_computeCallPointers.resize(count);
		for (auto& computeCall : s_staticComputeCalls)
			s_computeCallPointers[computeCall.callIndex] = &computeCall;
		for (auto& computeCall : s_dynamicComputeCalls)
			s_computeCallPointers[computeCall.callIndex] = &computeCall;

		return s_computeCallPointers;
	}
	void PostRender::ResetComputeCalls()
	{
		// Return all pShaderProperties of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : s_dynamicComputeCalls)
			PoolManager::ReturnShaderProperties(computeCall.pComputeShader, computeCall.pShaderProperties);

		// Remove all computeCalls so next frame can start fresh:
		s_staticComputeCalls.clear();
		s_dynamicComputeCalls.clear();
		s_computeCallPointers.clear();
		s_callIndex = 0;
	}
}