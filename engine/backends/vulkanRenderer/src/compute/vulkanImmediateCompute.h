#pragma once
#include "emberMath.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;



	class Immediate
	{
	private: // Members:
		static bool s_isInitialized;

	public: // Methods:
		static void Init();
		static void Clear();

		// Immediate dispatch call:
		static void Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount, float time, float deltaTime);

	private: // Methods:
		// Delete all constructors:
		Immediate() = delete;
		Immediate(const Immediate&) = delete;
		Immediate& operator=(const Immediate&) = delete;
		~Immediate() = delete;
	};
}