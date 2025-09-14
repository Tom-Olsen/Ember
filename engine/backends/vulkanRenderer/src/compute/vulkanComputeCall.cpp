#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanObjectToString.h"



namespace vulkanRendererBackend
{
	std::string ComputeCall::ToString()
	{
		if (pComputeShader == nullptr)
		{
			std::string dstName = objectToString::VkAccessFlagBits2ToString(dstAccessMask);
			std::string srcName = objectToString::VkAccessFlagBits2ToString(srcAccessMask);
			return "ComputeBarrier: dstAccessMask=" + dstName + ", srcAccessMask=" + srcName;
		}
		else
			return "ComputeShader: " + pComputeShader->GetName();
	}
}