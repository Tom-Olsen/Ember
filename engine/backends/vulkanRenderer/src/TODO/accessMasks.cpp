#include "accessMasks.h"
#include "vulkanAccessMasks.h"



namespace emberEngine
{
	VkAccessFlagBits2 AccessMask::Convert(ComputeShader flag)
	{
		switch (flag)
		{
			case ComputeShader::uniformRead: return vulkanBackend::accessMask::computeShader::uniformRead;
			case ComputeShader::shaderRead: return vulkanBackend::accessMask::computeShader::shaderRead;
			case ComputeShader::shaderWrite: return vulkanBackend::accessMask::computeShader::shaderWrite;
			case ComputeShader::sampledRead: return vulkanBackend::accessMask::computeShader::sampledRead;
			case ComputeShader::storageRead: return vulkanBackend::accessMask::computeShader::storageRead;
			case ComputeShader::storageWrite: return vulkanBackend::accessMask::computeShader::storageWrite;
			case ComputeShader::accelerationStructureRead: return vulkanBackend::accessMask::computeShader::accelerationStructureRead;
			case ComputeShader::descriptorBufferRead: return vulkanBackend::accessMask::computeShader::descriptorBufferRead;
			case ComputeShader::none: return vulkanBackend::accessMask::computeShader::none;
			default: return VkAccessFlagBits2(0);
		}
	}
}

