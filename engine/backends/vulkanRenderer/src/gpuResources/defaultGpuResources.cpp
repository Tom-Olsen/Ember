#include "defaultGpuResources.h"
#include "depthTexture2dArray.h"
#include "iMath.h"
#include "sampler.h"
#include "sampleTexture2d.h"
#include "storageBuffer.h"
#include "storageTexture2d.h"
#include "textureCube.h"



namespace vulkanRendererBackend
{
	// Static members:
	std::unique_ptr<Sampler> DefaultGpuResources::s_pColorSampler = nullptr;
	std::unique_ptr<Sampler> DefaultGpuResources::s_pShadowSampler = nullptr;
	std::unique_ptr<StorageBuffer> DefaultGpuResources::s_pDefaultStorageBuffer = nullptr;
	std::unique_ptr<SampleTexture2d> DefaultGpuResources::s_pDefaultSampleTexture2d = nullptr;
	std::unique_ptr<SampleTexture2d> DefaultGpuResources::s_pNormalMapSampleTexture2d = nullptr;
	std::unique_ptr<TextureCube> DefaultGpuResources::s_pDefaultTextureCube = nullptr;
	std::unique_ptr<DepthTexture2dArray> DefaultGpuResources::s_pDefaultDepthTexture2dArray = nullptr;
	std::unique_ptr<StorageTexture2d> DefaultGpuResources::s_pDefaultStorageTexture2d = nullptr;



	// Initialization/Cleanup:
	void DefaultGpuResources::Init()
	{
		s_pColorSampler = std::unique_ptr<Sampler>(Sampler::ColorSampler("colorSampler"));
		s_pShadowSampler = std::unique_ptr<Sampler>(Sampler::ShadowSampler("shadowSampler"));
		s_pDefaultStorageBuffer = std::make_unique<StorageBuffer>(1, 1, "1x1Dummy");
		s_pDefaultSampleTexture2d = std::make_unique<SampleTexture2d>("white", iMath::Float4One);
		s_pNormalMapSampleTexture2d = std::make_unique<SampleTexture2d>("defaultNormalMap", iMath::Float4Up);
		s_pDefaultTextureCube = std::make_unique<TextureCube>("whiteSkybox", iMath::Float4One);
		s_pDefaultDepthTexture2dArray = std::make_unique<DepthTexture2dArray>("defaultArrayTexture2d", VK_FORMAT_D32_SFLOAT, 2, 1, 1);
		s_pDefaultStorageTexture2d = std::make_unique<StorageTexture2d>("defaultStorageTexture", VK_FORMAT_R32G32B32A32_SFLOAT, 1, 1);
	}
	void DefaultGpuResources::Clear()
	{
		s_pColorSampler.reset();
		s_pShadowSampler.reset();
		s_pDefaultStorageBuffer.reset();
		s_pDefaultSampleTexture2d.reset();
		s_pNormalMapSampleTexture2d.reset();
		s_pDefaultTextureCube.reset();
		s_pDefaultDepthTexture2dArray.reset();
		s_pDefaultStorageTexture2d.reset();
	}


	// Public methods:
	Sampler* DefaultGpuResources::GetColorSampler()
	{
		return s_pColorSampler.get();
	}
	Sampler* DefaultGpuResources::GetShadowSampler()
	{
		return s_pShadowSampler.get();
	}
	StorageBuffer* DefaultGpuResources::GetDefaultStorageBuffer()
	{
		return s_pDefaultStorageBuffer.get();
	}
	SampleTexture2d* DefaultGpuResources::GetDefaultSampleTexture2d()
	{
		return s_pDefaultSampleTexture2d.get();
	}
	SampleTexture2d* DefaultGpuResources::GetNormalMapSampleTexture2d()
	{
		return s_pNormalMapSampleTexture2d.get();
	}
	TextureCube* DefaultGpuResources::GetDefaultTextureCube()
	{
		return s_pDefaultTextureCube.get();
	}
	DepthTexture2dArray* DefaultGpuResources::GetDefaultDepthTexture2dArray()
	{
		return s_pDefaultDepthTexture2dArray.get();
	}
	StorageTexture2d* DefaultGpuResources::GetDefaultStorageTexture2d()
	{
		return s_pDefaultStorageTexture2d.get();
	}
}