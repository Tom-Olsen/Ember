#ifndef __INCLUDE_GUARD_compute_h__
#define __INCLUDE_GUARD_compute_h__
#include "computeCall.h"
#include "mathf.h"
#include "resourcePool.h"
#include <unordered_map>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;

	class Compute
	{
	private: // Members
		static bool s_isInitialized;
		static uint32_t s_callIndex;
		static std::vector<ComputeCall> s_staticComputeCalls;
		static std::vector<ComputeCall> s_dynamicComputeCalls;
		static std::vector<ComputeCall*> s_computeCallPointers;
		static std::unordered_map<ComputeShader*, ResourcePool<ShaderProperties, 10>> s_shaderPropertiesPoolMap;

	public: // Methods
		static void Init();
		static void Clear();

		// Dispatch calls:
		static ShaderProperties* Dispatch(ComputeShader* pComputeShader, Uint3 threadCount);
		static ShaderProperties* Dispatch(ComputeShader* pComputeShadr, uint32_t threadCountX, uint32_t threadCountY, uint32_t threadCountZ);
		static void Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount);
		static void Dispatch(ComputeShader* pComputeShadr, ShaderProperties* pShaderProperties, uint32_t threadCountX, uint32_t threadCountY, uint32_t threadCountZ);
		
		// Barriers:
		static void Barrier(VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);


		// Management:
		static std::vector<ComputeCall*>* GetComputeCallPointers();
		static void ResetComputeCalls();

	private: // Methods
		// Delete all constructors:
		Compute() = delete;
		Compute(const Compute&) = delete;
		Compute& operator=(const Compute&) = delete;
		~Compute() = delete;
	};
}



#endif // __INCLUDE_GUARD_compute_h__