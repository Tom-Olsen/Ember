#ifndef __INCLUDE_GUARD_vulkanRendererBackend_immediateCompute_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_immediateCompute_h__
#include "computeCall.h"
#include "iMath.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;



	class Immediate
	{
	private: // Members
		static bool s_isInitialized;

	public: // Methods
		static void Init();
		static void Clear();

		// Immediate dispatch call:
		static void Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, iMath::Uint3 threadCount, float time, float deltaTime);

	private: // Methods
		// Delete all constructors:
		Immediate() = delete;
		Immediate(const Immediate&) = delete;
		Immediate& operator=(const Immediate&) = delete;
		~Immediate() = delete;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_immediateCompute_h__