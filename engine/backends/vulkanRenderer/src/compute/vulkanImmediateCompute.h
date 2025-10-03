#pragma once
#include "iCompute.h"
#include "emberMath.h"
#include "vulkanRendererExport.h"



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IShaderProperties;
}



namespace vulkanRendererBackend
{
	class VULKAN_RENDERER_API Immediate : public emberBackendInterface::ICompute::IImmediate
	{
	public: // Methods:
		// Constructor/Destructor:
		Immediate();
		~Immediate();

		// Non-copyable:
		Immediate(const Immediate&) = delete;
		Immediate& operator=(const Immediate&) = delete;

		// Movable:
		Immediate(Immediate&& other) noexcept;
		Immediate& operator=(Immediate&& other) noexcept;

		// Immediate dispatch call:
		void Dispatch(emberBackendInterface::IComputeShader* pComputeShader, emberBackendInterface::IShaderProperties* pShaderProperties, Uint3 threadCount, float time, float deltaTime) override;
	};
}