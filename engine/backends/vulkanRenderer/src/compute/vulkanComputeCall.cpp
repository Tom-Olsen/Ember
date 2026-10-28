#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"



namespace vulkanRendererBackend
{
	// Getters:
	bool ComputeCall::IsDispatch() const
	{
		return !computeShaderHandle.IsEmpty();
	}
	bool ComputeCall::IsBarrier() const
	{
		return !IsDispatch() && std::holds_alternative<std::monostate>(bufferCommand);
	}
	bool ComputeCall::IsBufferCopy() const
	{
		return std::holds_alternative<BufferCopy>(bufferCommand);
	}
	bool ComputeCall::IsBufferBarrier() const
	{
		return std::holds_alternative<BufferBarrier>(bufferCommand);
	}
	ComputeShader* ComputeCall::GetComputeShader() const
	{
		return static_cast<ComputeShader*>(computeShaderHandle.Get());
	}
	const ComputeCall::BufferCopy& ComputeCall::GetBufferCopy() const
	{
		return std::get<BufferCopy>(bufferCommand);
	}
	const ComputeCall::BufferBarrier& ComputeCall::GetBufferBarrier() const
	{
		return std::get<BufferBarrier>(bufferCommand);
	}



	// Debugging:
	std::string ComputeCall::ToString() const
	{
		if (IsBufferCopy())
			return "BufferCopy: size=" + std::to_string(GetBufferCopy().size);
		if (IsBufferBarrier())
			return "BufferBarrier: size=" + std::to_string(GetBufferBarrier().size);
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