#pragma once
#include "emberMath.h"
#include "vulkanAccessMask.h"
#include <vector>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;
	struct ComputeCall;



	class PreRender
	{
	private: // Members:
		static bool s_isInitialized;
		static uint32_t s_callIndex;
		static std::vector<ComputeCall> s_staticComputeCalls;
		static std::vector<ComputeCall> s_dynamicComputeCalls;
		static std::vector<ComputeCall*> s_computeCallPointers;

	public: // Methods:
		static void Init();
		static void Clear();

		// Recording:
		static ShaderProperties* RecordComputeShader(ComputeShader* pComputeShader, Uint3 threadCount);
		static void RecordComputeShader(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount);
		static void RecordBarrier(AccessMask srcAccessMask, AccessMask dstAccessMask);

		// Management:
		static std::vector<ComputeCall*>& GetComputeCallPointers();
		static void ResetComputeCalls();

	private: // Methods:
		// Delete all constructors:
		PreRender() = delete;
		PreRender(const PreRender&) = delete;
		PreRender& operator=(const PreRender&) = delete;
		~PreRender() = delete;
	};
}