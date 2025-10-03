#pragma once
#include "emberMath.h"
#include "iComputeShader.h"
#include "vulkanRendererExport.h"
#include "vulkanShader.h"
#include <string>



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

		// Non-copyable:
		ComputeShader(const ComputeShader&) = delete;
		ComputeShader& operator=(const ComputeShader&) = delete;

		// Movable:
		ComputeShader(ComputeShader&& other) noexcept;
		ComputeShader& operator=(ComputeShader&& other) noexcept;

		// Getters:
		Uint3 GetBlockSize() const override;
		const std::string& GetName() const override;
	};
}