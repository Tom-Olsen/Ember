#ifndef __INCLUDE_GUARD_compute_h__
#define __INCLUDE_GUARD_compute_h__
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ComputeUnit;
	class ShaderProperties;
	struct VulkanContext;

	class Compute
	{
	public: // Members

	private: // Members
		static uint32_t s_dispatchIndex;
		static bool s_isInitialized;
		static std::vector<ComputeUnit*> s_computeUnit;

	public: // Methods
		static void Init(VulkanContext* pContext);
		static void Clear();

		// Dispatch calls:
		static ShaderProperties* Dispatch(ComputeShader* pComputeShader, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

		static void ResetDispatchCalls();

		// Getters:
		static std::vector<ComputeUnit*>* GetComputeUnits();

	private: // Methods
		static void DoubleCapacityIfNeeded();
		static void ReduceCapacity();

		// Delete all constructors:
		Compute() = delete;
		Compute(const Compute&) = delete;
		Compute& operator=(const Compute&) = delete;
		~Compute() = delete;
	};
}



#endif // __INCLUDE_GUARD_compute_h__