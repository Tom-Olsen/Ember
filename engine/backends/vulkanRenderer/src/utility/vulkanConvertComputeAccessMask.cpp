#include "vulkanConvertComputeAccessMask.h"



namespace vulkanRendererBackend
{
	AccessMask ComputeAccessFlagsCommonToVulkan(emberCommon::ComputeAccessFlag accessFlags)
	{
		AccessMask accessMask = AccessMasks::ComputeShader::none;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::uniformRead))
			accessMask |= AccessMasks::ComputeShader::uniformRead;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::shaderRead))
			accessMask |= AccessMasks::ComputeShader::shaderRead;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::shaderWrite))
			accessMask |= AccessMasks::ComputeShader::shaderWrite;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::sampledRead))
			accessMask |= AccessMasks::ComputeShader::sampledRead;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::storageRead))
			accessMask |= AccessMasks::ComputeShader::storageRead;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::storageWrite))
			accessMask |= AccessMasks::ComputeShader::storageWrite;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::memoryRead))
			accessMask |= AccessMasks::ComputeShader::memoryRead;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::memoryWrite))
			accessMask |= AccessMasks::ComputeShader::memoryWrite;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::accelerationStructureRead))
			accessMask |= AccessMasks::ComputeShader::accelerationStructureRead;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::descriptorBufferRead))
			accessMask |= AccessMasks::ComputeShader::descriptorBufferRead;
		return accessMask;
	}
}