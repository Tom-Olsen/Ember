#ifndef __INCLUDE_GUARD_unknown_h__
#define __INCLUDE_GUARD_unknown_h__
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	struct AccessMask
	{
		enum class TopOfPipe
		{
			none
		};
		enum class ComputeShader
		{
			uniformRead,
			shaderRead,
			shaderWrite,
			sampledRead,
			storageRead,
			storageWrite,
			accelerationStructureRead,
			descriptorBufferRead,
			none
		};
		// Add more when needed, see vulkanAccessMasks.h

		static VkAccessFlagBits2 Convert(ComputeShader flag);
	};
}



#endif // __INCLUDE_GUARD_unknown_h__