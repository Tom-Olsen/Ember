#pragma once
#include "iDefaultGpuResources.h"
#include "vulkanRendererExport.h"
#include <cstdint>
#include <memory>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IMaterial;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	class DepthTexture2dArray;
	class DescriptorSetBinding;
	class Material;
	class Sampler;
	class SampleTextureCube;
	class SampleTexture2d;
	class SampleTexture3d;
	class StorageBuffer;
	class StorageTexture2d;
	class StorageTexture3d;



	class VULKAN_RENDERER_API DefaultGpuResources : public emberBackendInterface::IDefaultGpuResources
	{
	private: // Members:
		static DefaultGpuResources* s_pActiveInstance;
		// Samplers:
		std::unique_ptr<Sampler> m_pColorSampler;
		std::unique_ptr<Sampler> m_pColorSamplerClampEdge;
		std::unique_ptr<Sampler> m_pColorSamplerClampBorder;
		std::unique_ptr<Sampler> m_pShadowSampler;
		// Materials:
		std::unique_ptr<Material> m_pDefaultOutlineMaterial;
		std::unique_ptr<Material> m_pDefaultShadowMaterial;
		std::unique_ptr<Material> m_pDefaultDeferredLightingMaterial;
		std::unique_ptr<Material> m_pDefaultPresentMaterial;
		// Compute Shaders:
		std::unique_ptr<ComputeShader> m_pGammaCorrectionComputeShader;
		std::unique_ptr<ComputeShader> m_pOutlineCompositeComputeShader;
		std::unique_ptr<ComputeShader> m_pOutlineHorizontalMaskExpansionComputeShader;
		std::unique_ptr<ComputeShader> m_pOutlineVerticalMaskExpansionComputeShader;
		// Buffers:
		std::unique_ptr<StorageBuffer> m_pDefaultStorageBuffer;
		// Textures:
		std::unique_ptr<SampleTexture2d> m_pDefaultSampleTexture2d;
		std::unique_ptr<SampleTexture2d> m_pDefaultNormalMap;
		std::unique_ptr<SampleTexture3d> m_pDefaultSampleTexture3d;
		std::unique_ptr<SampleTextureCube> m_pDefaultSampleTextureCube;
		std::unique_ptr<DepthTexture2dArray> m_pDefaultDepthTexture2dArray;
		std::unique_ptr<StorageTexture2d> m_pDefaultStorageTexture2d;
		std::unique_ptr<StorageTexture3d> m_pDefaultStorageTexture3d;

	public: // Methods:
		// Constructor/Destructor:
		DefaultGpuResources();
		~DefaultGpuResources() override;

		// Non-copyable:
		DefaultGpuResources(const DefaultGpuResources&) = delete;
		DefaultGpuResources& operator=(const DefaultGpuResources&) = delete;

		// Non-movable:
		DefaultGpuResources(DefaultGpuResources&&) = delete;
		DefaultGpuResources& operator=(DefaultGpuResources&&) = delete;

		// Getters:
		// Singleton:
		static DefaultGpuResources& Get();
		
		// Samplers:
		Sampler* GetColorSampler();
		Sampler* GetColorSamplerClampEdge();
		Sampler* GetColorSamplerClampBorder();
		Sampler* GetShadowSampler();

		// Materials:
		Material* GetDefaultOutlineMaterial();
		Material* GetDefaultShadowMaterial();
		Material* GetDefaultDeferredLightingMaterial();
		Material* GetDefaultPresentMaterial();

		// Compute shaders:
		ComputeShader* GetGammaCorrectionComputeShader();
		ComputeShader* GetOutlineCompositeComputeShader();
		ComputeShader* GetOutlineHorizontalMaskExpansionComputeShader();
		ComputeShader* GetOutlineVerticalMaskExpansionComputeShader();

		// Buffers:
		StorageBuffer* GetDefaultStorageBuffer();

		// Textures:
		SampleTexture2d* GetDefaultSampleTexture2d();
		SampleTexture2d* GetDefaultNormalMap();
		SampleTexture3d* GetDefaultSampleTexture3d();
		SampleTextureCube* GetDefaultSampleTextureCube();
		DepthTexture2dArray* GetDefaultDepthTexture2dArray();
		StorageTexture2d* GetDefaultStorageTexture2d();
		StorageTexture3d* GetDefaultStorageTexture3d();

		// Built-in materials:
		void InitializeBuiltInMaterials(
			std::unique_ptr<emberBackendInterface::IMaterial> pOutlineMaterial,
			std::unique_ptr<emberBackendInterface::IMaterial> pDefaultShadowMaterial,
			std::unique_ptr<emberBackendInterface::IMaterial> pDeferredLightingMaterial,
			std::unique_ptr<emberBackendInterface::IMaterial> pPresentMaterial) override;

		// Built-in compute shaders:
		void InitializeBuiltInComputeShaders(
			std::unique_ptr<emberBackendInterface::IComputeShader> pGammaCorrectionComputeShader,
			std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineCompositeComputeShader,
			std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineHorizontalMaskExpansionComputeShader,
			std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineVerticalMaskExpansionComputeShader) override;
	};
}