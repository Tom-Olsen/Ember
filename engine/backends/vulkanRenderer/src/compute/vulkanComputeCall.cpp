#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"



namespace vulkanRendererBackend
{
	ComputeShader* ComputeCall::GetComputeShader() const
	{
		return static_cast<ComputeShader*>(computeShaderHandle.Get());
	}
	bool ComputeCall::IsBarrier() const
	{
		return computeShaderHandle.IsEmpty();
	}
	std::string ComputeCall::ToString() const
	{
		if (IsBarrier())
		{
			std::string dstName = AccessMasks::ToString(dstAccessMask);
			std::string srcName = AccessMasks::ToString(srcAccessMask);
			return "ComputeBarrier: dstAccessMask=" + dstName + ", srcAccessMask=" + srcName;
		}
		else
			return "ComputeShader: " + GetComputeShader()->GetDebugName();
	}
}