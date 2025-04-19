#ifndef __INCLUDE_GUARD_preRenderCompute_h__
#define __INCLUDE_GUARD_preRenderCompute_h__
#include "emberMath.h"
#include "accessMasks.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;



	namespace compute
	{
		// Forward declarations:
		struct ComputeCall;



		class PreRender
		{
		private: // Members
			static bool s_isInitialized;
			static uint32_t s_callIndex;
			static std::vector<ComputeCall> s_staticComputeCalls;
			static std::vector<ComputeCall> s_dynamicComputeCalls;
			static std::vector<ComputeCall*> s_computeCallPointers;

		public: // Methods
			static void Init();
			static void Clear();

			// Workload recording:
			static ShaderProperties* RecordComputeShader(ComputeShader* pComputeShader, Uint3 threadCount);
			static void RecordComputeShader(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount);
			static void RecordBarrier(AccessMask::ComputeShader srcAccessMask, AccessMask::ComputeShader dstAccessMask);

			// Management:
			static std::vector<ComputeCall*>& GetComputeCallPointers();
			static void ResetComputeCalls();

		private: // Methods
			// Delete all constructors:
			PreRender() = delete;
			PreRender(const PreRender&) = delete;
			PreRender& operator=(const PreRender&) = delete;
			~PreRender() = delete;
		};
	}
}



#endif // __INCLUDE_GUARD_preRenderCompute_h__