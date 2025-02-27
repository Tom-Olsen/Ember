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
		static std::vector<ComputeCall> s_computeCalls;
		static std::vector<ComputeCall*> s_computeCallPointers;
		static std::unordered_map<ComputeShader*, ResourcePool<ShaderProperties, 10>> s_shaderPropertiesPoolMap;

	public: // Methods
		static void Init();
		static void Clear();

		// Dispatch calls:
		static ShaderProperties* Dispatch(ComputeShader* pComputeShader, Uint3 threadCount);
		static ShaderProperties* Dispatch(ComputeShader* pComputeShader, uint32_t threadCountX, uint32_t threadCountY, uint32_t threadCountZ);
		
		static void ResetComputeCalls();

		// Getters:
		static std::vector<ComputeCall*>* GetComputeCallPointers();

	private: // Methods
		// Delete all constructors:
		Compute() = delete;
		Compute(const Compute&) = delete;
		Compute& operator=(const Compute&) = delete;
		~Compute() = delete;
	};
}



#endif // __INCLUDE_GUARD_compute_h__