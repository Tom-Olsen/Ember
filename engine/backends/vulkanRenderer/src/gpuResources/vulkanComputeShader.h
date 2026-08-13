#pragma once
#include "emberMath.h"
#include "iComputeShader.h"
#include "vulkanRendererExport.h"
#include "vulkanShader.h"
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class DescriptorSetBinding;
	class Pipeline;


    
	class VULKAN_RENDERER_API ComputeShader : public Shader, public emberBackendInterface::IComputeShader
	{
	private: // Members:
		Uint3 m_blockSize;
		std::unique_ptr<DescriptorSetBinding> m_pShaderDescriptorSetBinding;
		std::unique_ptr<Pipeline> m_pPipeline;

	public: // Methods:
		// Constructors/Destructor:
		ComputeShader(const std::filesystem::path& computeSpv, const std::string& debugName);
		~ComputeShader();

		// Non-copyable:
		ComputeShader(const ComputeShader&) = delete;
		ComputeShader& operator=(const ComputeShader&) = delete;

		// Movable:
		ComputeShader(ComputeShader&& other) noexcept;
		ComputeShader& operator=(ComputeShader&& other) noexcept;

		// Getters:
		Uint3 GetBlockSize() const override;
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;
		DescriptorSetBinding* GetDescriptorSetBinding() const;
		const Pipeline* GetPipeline() const;

		// Debugging:
		void Print() const override;
	};
}