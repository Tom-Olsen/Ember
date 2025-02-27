#ifndef __INCLUDE_GUARD_computeCall_h__
#define __INCLUDE_GUARD_computeCall_h__
#include "macros.h"
#include "mathf.h"
#include <array>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;

	struct ComputeCall
	{
		Uint3 threadCount;
		ComputeShader* pComputeShader;
		ShaderProperties* pShaderProperties;
	};
}



#endif // __INCLUDE_GUARD_computeCall_h__