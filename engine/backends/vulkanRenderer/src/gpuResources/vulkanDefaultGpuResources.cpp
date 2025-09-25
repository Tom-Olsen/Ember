#include "vulkanDefaultGpuResources.h"
#include "emberMath.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanColorSampler.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanShadowSampler.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageTexture2d.h"



namespace vulkanRendererBackend
{
	// Static members:
	std::unique_ptr<Sampler> DefaultGpuResources::s_pColorSampler = nullptr;
	std::unique_ptr<Sampler> DefaultGpuResources::s_pShadowSampler = nullptr;
	std::unique_ptr<StorageBuffer> DefaultGpuResources::s_pDefaultStorageBuffer = nullptr;
	std::unique_ptr<SampleTexture2d> DefaultGpuResources::s_pDefaultSampleTexture2d = nullptr;
	std::unique_ptr<SampleTexture2d> DefaultGpuResources::s_pNormalMapSampleTexture2d = nullptr;
	std::unique_ptr<SampleTextureCube> DefaultGpuResources::s_pDefaultTextureCube = nullptr;
	std::unique_ptr<DepthTexture2dArray> DefaultGpuResources::s_pDefaultDepthTexture2dArray = nullptr;
	std::unique_ptr<StorageTexture2d> DefaultGpuResources::s_pDefaultStorageTexture2d = nullptr;



	// Initialization/Cleanup:
	void DefaultGpuResources::Init()
	{
		s_pColorSampler = std::make_unique<ColorSampler>("colorSampler");
		s_pShadowSampler = std::make_unique<ShadowSampler>("shadowSampler");
		s_pDefaultStorageBuffer = std::make_unique<StorageBuffer>(1, 1, "1x1Dummy");
		s_pDefaultSampleTexture2d = std::make_unique<SampleTexture2d>("white", Formats::r8g8b8a8_unorm, 1, 1, (void*)&Float4::white);
		s_pNormalMapSampleTexture2d = std::make_unique<SampleTexture2d>("defaultNormalMap", Formats::r8g8b8a8_unorm, 1, 1, (void*)&Float4::up);
        std::array<Float4, 6> whiteFaces = { Float4::white, Float4::white, Float4::white, Float4::white, Float4::white, Float4::white };
		s_pDefaultTextureCube = std::make_unique<SampleTextureCube>("whiteSkybox", Formats::r32g32b32a32_sfloat, 1, 1, whiteFaces.data());
		s_pDefaultDepthTexture2dArray = std::make_unique<DepthTexture2dArray>("defaultArrayTexture2d", Formats::d32_sfloat, 2, 1, 1);
		s_pDefaultStorageTexture2d = std::make_unique<StorageTexture2d>("defaultStorageTexture", Formats::r32g32b32a32_sfloat, 1, 1, (void*)&Float4::one);
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
	SampleTextureCube* DefaultGpuResources::GetDefaultTextureCube()
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