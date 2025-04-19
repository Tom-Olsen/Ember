#ifndef __INCLUDE_GUARD_postRenderCompute_h__
#define __INCLUDE_GUARD_postRenderCompute_h__
#include "emberMath.h"
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;



	namespace compute
	{
		// Forward declarations:
		struct ComputeCall;



		class PostRender
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
			static ShaderProperties* RecordComputeShader(ComputeShader* pComputeShader);
			static void RecordComputeShader(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties);
			
			// Management:
			static std::vector<ComputeCall*>& GetComputeCallPointers();
			static void ResetComputeCalls();

		private: // Methods
			// Delete all constructors:
			PostRender() = delete;
			PostRender(const PostRender&) = delete;
			PostRender& operator=(const PostRender&) = delete;
			~PostRender() = delete;
		};
	}
}



#endif // __INCLUDE_GUARD_postRenderCompute_h__