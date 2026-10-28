#include "vulkanConvertComputeAccessMask.h"



namespace vulkanRendererBackend
{
	AccessMask ComputeBarrierFlagsToVulkanAccessMask(emberCommon::ComputeBarrierFlag barrierFlags)
	{
		AccessMask accessMask = AccessMasks::ComputeShader::none;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::uniformRead))
			accessMask |= AccessMasks::ComputeShader::uniformRead;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::shaderRead))
			accessMask |= AccessMasks::ComputeShader::shaderRead;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::shaderWrite))
			accessMask |= AccessMasks::ComputeShader::shaderWrite;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::sampledRead))
			accessMask |= AccessMasks::ComputeShader::sampledRead;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::storageRead))
			accessMask |= AccessMasks::ComputeShader::storageRead;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::storageWrite))
			accessMask |= AccessMasks::ComputeShader::storageWrite;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::memoryRead))
			accessMask |= AccessMasks::ComputeShader::memoryRead;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::memoryWrite))
			accessMask |= AccessMasks::ComputeShader::memoryWrite;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::accelerationStructureRead))
			accessMask |= AccessMasks::ComputeShader::accelerationStructureRead;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::descriptorBufferRead))
			accessMask |= AccessMasks::ComputeShader::descriptorBufferRead;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::transferRead))
			accessMask |= AccessMasks::Transfer::transferRead;
		if (emberCommon::HasFlag(barrierFlags, emberCommon::ComputeBarrierFlag::transferWrite))
			accessMask |= AccessMasks::Transfer::transferWrite;
		return accessMask;
	}
}