#include "computeCall.h"
#include "computeShader.h"
#include "logger.h"
#include "vulkanObjectToString.h"



namespace emberEngine
{
	namespace compute
	{
		std::string ComputeCall::ToString()
		{
			if (pComputeShader == nullptr)
			{
				std::string dstName = vulkanBackend::objectToString::VkAccessFlagBits2ToString(dstAccessMask);
				std::string srcName = vulkanBackend::objectToString::VkAccessFlagBits2ToString(srcAccessMask);
				return "ComputeBarrier: dstAccessMask=" + dstName + ", srcAccessMask=" + srcName;
			}
			else
				return "ComputeShader: " + pComputeShader->GetName();
		}
	}
}