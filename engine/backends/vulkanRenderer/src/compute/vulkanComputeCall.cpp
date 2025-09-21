#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	std::string ComputeCall::ToString()
	{
		if (pComputeShader == nullptr)
		{
			std::string dstName = AccessMasks::ToString(dstAccessMask);
			std::string srcName = AccessMasks::ToString(srcAccessMask);
			return "ComputeBarrier: dstAccessMask=" + dstName + ", srcAccessMask=" + srcName;
		}
		else
			return "ComputeShader: " + pComputeShader->GetName();
	}
}