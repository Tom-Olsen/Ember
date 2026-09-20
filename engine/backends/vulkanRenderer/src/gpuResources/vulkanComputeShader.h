#pragma once
#include "iComputeShader.h"
#include "commonComputeShaderFeatures.h"
#include "emberMath.h"
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
	class ComputeCallQueue;
	class ComputeQueue;
	class DescriptorSetBinding;
	class Pipeline;
	class Renderer;



	class VULKAN_RENDERER_API ComputeShader : public Shader, public emberBackendInterface::IComputeShader
	{
		// Friends:
		friend class Async;
		friend class ComputeCallQueue;
		friend class ComputeQueue;
		friend class ComputeShaderManager;
		friend class Renderer;

	private: // Members:
		Uint3 m_blockSize;
		emberCommon::ComputeShaderFeatures m_features;
		std::unique_ptr<DescriptorSetBinding> m_pShaderDescriptorSetBinding;
		std::unique_ptr<Pipeline> m_pPipeline;
		uint32_t m_pendingUseCount = 0;

	public: // Methods:
		// Destructor:
		~ComputeShader();

		// Non-copyable:
		ComputeShader(const ComputeShader&) = delete;
		ComputeShader& operator=(const ComputeShader&) = delete;

		// Movable:
		ComputeShader(ComputeShader&& other) = delete;
		ComputeShader& operator=(ComputeShader&& other) = delete;

		// Getters:
		Uint3 GetBlockSize() const override;
		const emberCommon::ComputeShaderFeatures& GetFeatures() const;
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;
		DescriptorSetBinding* GetDescriptorSetBinding() const;
		const Pipeline* GetPipeline() const;

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Constructor:
		ComputeShader(const std::filesystem::path& computeSpv, const emberCommon::ComputeShaderFeatures& features, const std::string& debugName);
		
		// Use management:
		void AddPendingUse();
		void RemovePendingUse();
		bool HasPendingUse() const;
	};
}