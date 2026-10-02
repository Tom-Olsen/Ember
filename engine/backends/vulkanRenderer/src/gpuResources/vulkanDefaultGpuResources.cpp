#include "vulkanDefaultGpuResources.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "iMaterial.h"
#include "iRenderer.h"
#include "vulkanColorSampler.h"
#include "vulkanComputeShader.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanMaterial.h"
#include "vulkanSampler.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSampleTexture3d.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanShadowSampler.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanStorageTexture3d.h"
#include <array>
#include <filesystem>
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Static members:
	DefaultGpuResources* DefaultGpuResources::s_pActiveInstance = nullptr;



	// Public methods:
	// Constructor/Destructor:
	DefaultGpuResources::DefaultGpuResources()
	{
		if (s_pActiveInstance != nullptr)
			throw std::runtime_error("DefaultGpuResources constructor failed. An active instance already exists.");

		s_pActiveInstance = this;
		try
		{
			// Samplers:
			ColorSampler::Settings colorSamplerSettings;
			colorSamplerSettings.name = "Sampler_Color";
			m_pColorSampler = std::make_unique<ColorSampler>(colorSamplerSettings);

			ColorSampler::Settings colorSamplerClampEdgeSettings;
			colorSamplerClampEdgeSettings.name = "Sampler_ColorClampEdge";
			colorSamplerClampEdgeSettings.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			m_pColorSamplerClampEdge = std::make_unique<ColorSampler>(colorSamplerClampEdgeSettings);
			m_pShadowSampler = std::make_unique<ShadowSampler>("Sampler_Shadow");

			// Buffers:
			m_pDefaultStorageBuffer = std::make_unique<StorageBuffer>(1, sizeof(int));

			// Textures:
			std::array<unsigned char, 4> whitePixel = { 255, 255, 255, 255 };
			std::array<unsigned char, 4> normalPixel = { 128, 128, 255, 255 };
			m_pDefaultSampleTexture2d = std::make_unique<SampleTexture2d>(VK_FORMAT_R8G8B8A8_UNORM, 1, 1, whitePixel.data());
			m_pDefaultNormalMap = std::make_unique<SampleTexture2d>(VK_FORMAT_R8G8B8A8_UNORM, 1, 1, normalPixel.data());
			m_pDefaultSampleTexture3d = std::make_unique<SampleTexture3d>(VK_FORMAT_R8G8B8A8_UNORM, 1, 1, 1, whitePixel.data());
			std::array<Float4, 6> whiteFaces = { Float4::white, Float4::white, Float4::white, Float4::white, Float4::white, Float4::white };
			m_pDefaultSampleTextureCube = std::make_unique<SampleTextureCube>(VK_FORMAT_R32G32B32A32_SFLOAT, 1, 1, whiteFaces.data());
			m_pDefaultDepthTexture2dArray = std::make_unique<DepthTexture2dArray>(VK_FORMAT_D32_SFLOAT, 2, 1, 1);
			m_pDefaultStorageTexture2d = std::make_unique<StorageTexture2d>(VK_FORMAT_R32G32B32A32_SFLOAT, 1, 1, (void*)&Float4::one);
			m_pDefaultStorageTexture3d = std::make_unique<StorageTexture3d>(VK_FORMAT_R32G32B32A32_SFLOAT, 1, 1, 1, (void*)&Float4::one);
		}
		catch (...)
		{
			s_pActiveInstance = nullptr;
			throw;
		}
	}
	DefaultGpuResources::~DefaultGpuResources()
	{
		// Materials:
		m_pDefaultPresentMaterial.reset();
		m_pDefaultDeferredLightingMaterial.reset();
		m_pDefaultShadowMaterial.reset();
		m_pDefaultOutlineMaterial.reset();

		// Compute shaders:
		m_pOutlineVerticalMaskExpansionComputeShader.reset();
		m_pOutlineHorizontalMaskExpansionComputeShader.reset();
		m_pOutlineCompositeComputeShader.reset();
		m_pGammaCorrectionComputeShader.reset();

		// Textures:
		m_pDefaultStorageTexture3d.reset();
		m_pDefaultStorageTexture2d.reset();
		m_pDefaultDepthTexture2dArray.reset();
		m_pDefaultSampleTextureCube.reset();
		m_pDefaultSampleTexture3d.reset();
		m_pDefaultNormalMap.reset();
		m_pDefaultSampleTexture2d.reset();
		
		// Buffers:
		m_pDefaultStorageBuffer.reset();

		// Samplers:
		m_pShadowSampler.reset();
		m_pColorSamplerClampEdge.reset();
		m_pColorSampler.reset();

		if (s_pActiveInstance == this)
			s_pActiveInstance = nullptr;
	}



	// Getters:
	// Singleton:
	DefaultGpuResources& DefaultGpuResources::Get()
	{
		if (s_pActiveInstance == nullptr)
			throw std::runtime_error("DefaultGpuResources::Get() failed. No active instance exists.");
		return *s_pActiveInstance;
	}
	// Samplers:
	Sampler* DefaultGpuResources::GetColorSampler()
	{
		return m_pColorSampler.get();
	}
	Sampler* DefaultGpuResources::GetColorSamplerClampEdge()
	{
		return m_pColorSamplerClampEdge.get();
	}
	Sampler* DefaultGpuResources::GetShadowSampler()
	{
		return m_pShadowSampler.get();
	}
	// Materials:
	Material* DefaultGpuResources::GetDefaultOutlineMaterial()
	{
		if (m_pDefaultOutlineMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetDefaultOutlineMaterial() failed. Default outline material is not set.");
		return m_pDefaultOutlineMaterial.get();
	}
	Material* DefaultGpuResources::GetDefaultShadowMaterial()
	{
		if (m_pDefaultShadowMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetDefaultShadowMaterial() failed. Default shadow material is not set.");
		return m_pDefaultShadowMaterial.get();
	}
	Material* DefaultGpuResources::GetDefaultDeferredLightingMaterial()
	{
		if (m_pDefaultDeferredLightingMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetDefaultDeferredLightingMaterial() failed. Default deferred lighting material is not set.");
		return m_pDefaultDeferredLightingMaterial.get();
	}
	Material* DefaultGpuResources::GetDefaultPresentMaterial()
	{
		if (m_pDefaultPresentMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetDefaultPresentMaterial() failed. Default present material is not set.");
		return m_pDefaultPresentMaterial.get();
	}
	// Compute shaders:
	ComputeShader* DefaultGpuResources::GetGammaCorrectionComputeShader()
	{
		if (m_pGammaCorrectionComputeShader == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetGammaCorrectionComputeShader() failed. Built-in gamma correction compute shader is not initialized.");
		return m_pGammaCorrectionComputeShader.get();
	}
	ComputeShader* DefaultGpuResources::GetOutlineCompositeComputeShader()
	{
		if (m_pOutlineCompositeComputeShader == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetOutlineCompositeComputeShader() failed. Built-in outline composite compute shader is not initialized.");
		return m_pOutlineCompositeComputeShader.get();
	}
	ComputeShader* DefaultGpuResources::GetOutlineHorizontalMaskExpansionComputeShader()
	{
		if (m_pOutlineHorizontalMaskExpansionComputeShader == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetOutlineHorizontalMaskExpansionComputeShader() failed. Built-in outline horizontal mask expansion compute shader is not initialized.");
		return m_pOutlineHorizontalMaskExpansionComputeShader.get();
	}
	ComputeShader* DefaultGpuResources::GetOutlineVerticalMaskExpansionComputeShader()
	{
		if (m_pOutlineVerticalMaskExpansionComputeShader == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetOutlineVerticalMaskExpansionComputeShader() failed. Built-in outline vertical mask expansion compute shader is not initialized.");
		return m_pOutlineVerticalMaskExpansionComputeShader.get();
	}
	// Buffers:
	StorageBuffer* DefaultGpuResources::GetDefaultStorageBuffer()
	{
		return m_pDefaultStorageBuffer.get();
	}
	// Textures:
	SampleTexture2d* DefaultGpuResources::GetDefaultSampleTexture2d()
	{
		return m_pDefaultSampleTexture2d.get();
	}
	SampleTexture2d* DefaultGpuResources::GetDefaultNormalMap()
	{
		return m_pDefaultNormalMap.get();
	}
	SampleTexture3d* DefaultGpuResources::GetDefaultSampleTexture3d()
	{
		return m_pDefaultSampleTexture3d.get();
	}
	SampleTextureCube* DefaultGpuResources::GetDefaultSampleTextureCube()
	{
		return m_pDefaultSampleTextureCube.get();
	}
	DepthTexture2dArray* DefaultGpuResources::GetDefaultDepthTexture2dArray()
	{
		return m_pDefaultDepthTexture2dArray.get();
	}
	StorageTexture2d* DefaultGpuResources::GetDefaultStorageTexture2d()
	{
		return m_pDefaultStorageTexture2d.get();
	}
	StorageTexture3d* DefaultGpuResources::GetDefaultStorageTexture3d()
	{
		return m_pDefaultStorageTexture3d.get();
	}




	// Built-in materials:
	void DefaultGpuResources::InitializeBuiltInMaterials(
		std::unique_ptr<emberBackendInterface::IMaterial> pOutlineMaterial,
		std::unique_ptr<emberBackendInterface::IMaterial> pDefaultShadowMaterial,
		std::unique_ptr<emberBackendInterface::IMaterial> pDeferredLightingMaterial,
		std::unique_ptr<emberBackendInterface::IMaterial> pPresentMaterial)
	{
		if (m_pDefaultOutlineMaterial != nullptr || m_pDefaultShadowMaterial != nullptr || m_pDefaultDeferredLightingMaterial != nullptr || m_pDefaultPresentMaterial != nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Built-in materials are already initialized.");
		if (pOutlineMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Outline material is null.");
		if (pDefaultShadowMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Default shadow material is null.");
		if (pDeferredLightingMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Deferred lighting material is null.");
		if (pPresentMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Present material is null.");
		if (pOutlineMaterial->GetMaterialPass() != emberCommon::MaterialPass::outline)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Outline material has wrong material pass.");
		if (pDefaultShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Default shadow material has wrong material pass.");
		if (pDeferredLightingMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Deferred lighting material has wrong material pass.");
		if (pPresentMaterial->GetMaterialPass() != emberCommon::MaterialPass::present)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInMaterials(...) failed. Present material has wrong material pass.");

		m_pDefaultOutlineMaterial.reset(static_cast<Material*>(pOutlineMaterial.release()));
		m_pDefaultShadowMaterial.reset(static_cast<Material*>(pDefaultShadowMaterial.release()));
		m_pDefaultDeferredLightingMaterial.reset(static_cast<Material*>(pDeferredLightingMaterial.release()));
		m_pDefaultPresentMaterial.reset(static_cast<Material*>(pPresentMaterial.release()));
	}



	// Initialize default compute shaders:
	void DefaultGpuResources::InitializeBuiltInComputeShaders(
		std::unique_ptr<emberBackendInterface::IComputeShader> pGammaCorrectionComputeShader,
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineCompositeComputeShader,
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineHorizontalMaskExpansionComputeShader,
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineVerticalMaskExpansionComputeShader)
	{
		if (m_pGammaCorrectionComputeShader != nullptr || m_pOutlineCompositeComputeShader != nullptr || m_pOutlineHorizontalMaskExpansionComputeShader != nullptr || m_pOutlineVerticalMaskExpansionComputeShader != nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInComputeShaders(...) failed. Built-in compute shaders are already initialized.");
		if (pGammaCorrectionComputeShader == nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInComputeShaders(...) failed. GammaCorrectionComputeShader compute shader is null.");
		if (pOutlineCompositeComputeShader == nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInComputeShaders(...) failed. OutlineCompositeComputeShader compute shader is null.");
		if (pOutlineHorizontalMaskExpansionComputeShader == nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInComputeShaders(...) failed. OutlineHorizontalMaskExpansionComputeShader compute shader is null.");
		if (pOutlineVerticalMaskExpansionComputeShader == nullptr)
			throw std::runtime_error("DefaultGpuResources::InitializeBuiltInComputeShaders(...) failed. OutlineVerticalMaskExpansionComputeShader compute shader is null.");

		m_pGammaCorrectionComputeShader.reset(static_cast<ComputeShader*>(pGammaCorrectionComputeShader.release()));
		m_pOutlineCompositeComputeShader.reset(static_cast<ComputeShader*>(pOutlineCompositeComputeShader.release()));
		m_pOutlineHorizontalMaskExpansionComputeShader.reset(static_cast<ComputeShader*>(pOutlineHorizontalMaskExpansionComputeShader.release()));
		m_pOutlineVerticalMaskExpansionComputeShader.reset(static_cast<ComputeShader*>(pOutlineVerticalMaskExpansionComputeShader.release()));
	}
}