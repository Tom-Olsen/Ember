#include "vulkanConvertComputeAccessMask.h"



namespace vulkanRendererBackend
{
	AccessMask ComputeBarrierFlagsToVulkanAccessMask(emberBackendInterface::ComputeBarrierFlag barrierFlags)
	{
		AccessMask accessMask = AccessMasks::ComputeShader::none;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::uniformRead))
			accessMask |= AccessMasks::ComputeShader::uniformRead;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::shaderRead))
			accessMask |= AccessMasks::ComputeShader::shaderRead;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::shaderWrite))
			accessMask |= AccessMasks::ComputeShader::shaderWrite;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::sampledRead))
			accessMask |= AccessMasks::ComputeShader::sampledRead;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::storageRead))
			accessMask |= AccessMasks::ComputeShader::storageRead;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::storageWrite))
			accessMask |= AccessMasks::ComputeShader::storageWrite;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::memoryRead))
			accessMask |= AccessMasks::ComputeShader::memoryRead;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::memoryWrite))
			accessMask |= AccessMasks::ComputeShader::memoryWrite;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::accelerationStructureRead))
			accessMask |= AccessMasks::ComputeShader::accelerationStructureRead;
		if (emberBackendInterface::HasFlag(barrierFlags, emberBackendInterface::ComputeBarrierFlag::descriptorBufferRead))
			accessMask |= AccessMasks::ComputeShader::descriptorBufferRead;
		return accessMask;
	}
}