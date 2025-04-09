#include "computeCall.h"
#include "computeShader.h"
#include "logger.h"
#include "vulkanUtility.h"



namespace emberEngine
{
	std::string ComputeCall::ToString()
	{
		if (pComputeShader == nullptr)
		{
			std::string dstName = vulkanObjToString::VkAccessFlagBits2ToString(dstAccessMask);
			std::string srcName = vulkanObjToString::VkAccessFlagBits2ToString(srcAccessMask);
			return "ComputeBarrier: dstAccessMask=" + dstName + ", srcAccessMask=" + srcName;
		}
		else
			return "ComputeShader: " + pComputeShader->GetName();
	}
}