#pragma once
#include "iCompute.h"
#include "emberMath.h"



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IShaderProperties;
}



namespace vulkanRendererBackend
{
	class Immediate : public emberBackendInterface::ICompute::IImmediate
	{
	public: // Methods:
		// Constructor/Destructor:
		Immediate();
		~Immediate();

		// Non-copyable:
		Immediate(const Immediate&) = delete;
		Immediate& operator=(const Immediate&) = delete;

		// Movable:
		Immediate(Immediate&& other) noexcept = default;
		Immediate& operator=(Immediate&& other) noexcept = default;

		// Immediate dispatch call:
		void Dispatch(emberBackendInterface::IComputeShader* pComputeShader, emberBackendInterface::IShaderProperties* pShaderProperties, Uint3 threadCount, float time, float deltaTime) override;
	};
}