#pragma once
#include "emberMath.h"
#include "iComputeShader.h"
#include "vulkanRendererExport.h"
#include "vulkanShader.h"
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	class VULKAN_RENDERER_API ComputeShader : public Shader, public emberBackendInterface::IComputeShader
	{
	private: // Members:
		Uint3 m_blockSize;

	public: // Methods:
		// Constructors/Destructor:
		ComputeShader(const std::string& name, const std::filesystem::path& computeSpv);
		~ComputeShader();

		// Getters:
		Uint3 GetBlockSize() const override;
	};
}