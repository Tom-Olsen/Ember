#ifndef __INCLUDE_GUARD_immediateCompute_h__
#define __INCLUDE_GUARD_immediateCompute_h__
#include "computeCall.h"
#include "emberMath.h"



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;



	namespace compute
	{
		class Immediate
		{
		private: // Members
			static bool s_isInitialized;

		public: // Methods
			static void Init();
			static void Clear();

			// Immediate dispatch call:
			static void Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount);

		private: // Methods
			// Delete all constructors:
			Immediate() = delete;
			Immediate(const Immediate&) = delete;
			Immediate& operator=(const Immediate&) = delete;
			~Immediate() = delete;
		};
	}
}



#endif // __INCLUDE_GUARD_immediateCompute_h__