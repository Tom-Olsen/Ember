#pragma once
#include "emberMath.h"
#include "iComputeShader.h"
#include "vulkanRendererExport.h"
#include "vulkanShader.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Async;
	class DescriptorSetBinding;
	class Pipeline;
	class PostRender;
	class PreRender;
	class Renderer;


    
	class VULKAN_RENDERER_API ComputeShader : public Shader, public emberBackendInterface::IComputeShader
	{
		// Friends:
		friend class Async;
		friend class PostRender;
		friend class PreRender;
		friend class Renderer;

	private: // Members:
		Uint3 m_blockSize;
		std::unique_ptr<DescriptorSetBinding> m_pShaderDescriptorSetBinding;
		std::unique_ptr<Pipeline> m_pPipeline;
		uint32_t m_pendingUseCount = 0;

	public: // Methods:
		// Constructors/Destructor:
		ComputeShader(const std::filesystem::path& computeSpv, const std::string& debugName);
		~ComputeShader();

		// Non-copyable:
		ComputeShader(const ComputeShader&) = delete;
		ComputeShader& operator=(const ComputeShader&) = delete;

		// Movable:
		ComputeShader(ComputeShader&& other) = delete;
		ComputeShader& operator=(ComputeShader&& other) = delete;

		// Getters:
		Uint3 GetBlockSize() const override;
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;
		DescriptorSetBinding* GetDescriptorSetBinding() const;
		const Pipeline* GetPipeline() const;

		// Debugging:
		void Print() const override;

	private: // Methods:
		void AddPendingUse();
		void RemovePendingUse();
		bool HasPendingUse() const;
	};
}