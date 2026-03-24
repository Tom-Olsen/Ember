#include "vulkanDefaultGpuResources.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "vulkanComputeShader.h"
#include "vulkanColorSampler.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanSampler.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanShadowSampler.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageTexture2d.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	// Static members:
	bool DefaultGpuResources::s_isInitialized = false;
	// Samplers:
	std::unique_ptr<Sampler> DefaultGpuResources::s_pColorSampler = nullptr;
	std::unique_ptr<Sampler> DefaultGpuResources::s_pShadowSampler = nullptr;
	// Materials:
	std::unique_ptr<Material> DefaultGpuResources::s_pDefaultPresentMaterial = nullptr;
	std::unique_ptr<Material> DefaultGpuResources::s_pDefaultShadowMaterial = nullptr;
	// Compute shaders:
	std::unique_ptr<ComputeShader> DefaultGpuResources::s_pGammaCorrectionComputeShader = nullptr;
	// Meshes:
	std::unique_ptr<Mesh> DefaultGpuResources::s_pDefaultRenderQuad = nullptr;
	// Buffers:
	std::unique_ptr<StorageBuffer> DefaultGpuResources::s_pDefaultStorageBuffer = nullptr;
	// Textures:
	std::unique_ptr<SampleTexture2d> DefaultGpuResources::s_pDefaultSampleTexture2d = nullptr;
	std::unique_ptr<SampleTexture2d> DefaultGpuResources::s_pNormalMapSampleTexture2d = nullptr;
	std::unique_ptr<SampleTextureCube> DefaultGpuResources::s_pDefaultSampleTextureCube = nullptr;
	std::unique_ptr<DepthTexture2dArray> DefaultGpuResources::s_pDefaultDepthTexture2dArray = nullptr;
	std::unique_ptr<StorageTexture2d> DefaultGpuResources::s_pDefaultStorageTexture2d = nullptr;



	// Initialization/Cleanup:
	void DefaultGpuResources::Init(uint32_t shadowMapResolution)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
		std::filesystem::path shadersSrcDirectory = (std::filesystem::path(ENGINE_SHADERS_DIR) / "shaders" / "src").make_preferred();

		// Samplers:
		s_pColorSampler = std::make_unique<ColorSampler>("colorSampler");
		s_pShadowSampler = std::make_unique<ShadowSampler>("shadowSampler");
		// Materials:
		s_pDefaultPresentMaterial = std::make_unique<Material>(Material::CreatePresent("presentMaterial", shadersSrcDirectory / "vertex" / "present.vert.spv", shadersSrcDirectory / "fragment" / "present.frag.spv"));
		s_pDefaultShadowMaterial = std::make_unique<Material>(Material::CreateShadow("shadowMaterial", shadowMapResolution));
		// Compute shaders:
		s_pGammaCorrectionComputeShader = std::make_unique<ComputeShader>("gammaCorrectionComputeShader", shadersSrcDirectory / "compute" / "gammaCorrection.comp.spv");
		// Meshes:
		s_pDefaultRenderQuad = std::make_unique<Mesh>(CreateDefaultRenderQuad());
		// Buffers:
		s_pDefaultStorageBuffer = std::make_unique<StorageBuffer>(1, sizeof(int));
		// Textures:
		s_pDefaultSampleTexture2d = std::make_unique<SampleTexture2d>("white", Formats::r8g8b8a8_unorm, 1, 1, (void*)&Float4::white);
		s_pNormalMapSampleTexture2d = std::make_unique<SampleTexture2d>("defaultNormalMap", Formats::r8g8b8a8_unorm, 1, 1, (void*)&Float4::up);
        std::array<Float4, 6> whiteFaces = { Float4::white, Float4::white, Float4::white, Float4::white, Float4::white, Float4::white };
		s_pDefaultSampleTextureCube = std::make_unique<SampleTextureCube>("whiteSkybox", Formats::r32g32b32a32_sfloat, 1, 1, whiteFaces.data());
		s_pDefaultDepthTexture2dArray = std::make_unique<DepthTexture2dArray>("defaultArrayTexture2d", Formats::d32_sfloat, 2, 1, 1);
		s_pDefaultStorageTexture2d = std::make_unique<StorageTexture2d>("defaultStorageTexture", Formats::r32g32b32a32_sfloat, 1, 1, (void*)&Float4::one);
	}
	void DefaultGpuResources::Clear()
	{
		// Samplers:
		s_pColorSampler.reset();
		s_pShadowSampler.reset();
		// Materials:
		s_pDefaultPresentMaterial.reset();
		s_pDefaultShadowMaterial.reset();
		// Compute shaders:
		s_pGammaCorrectionComputeShader.reset();
		// Meshes:
		s_pDefaultRenderQuad.reset();
		// Buffers:
		s_pDefaultStorageBuffer.reset();
		// Textures:
		s_pDefaultSampleTexture2d.reset();
		s_pNormalMapSampleTexture2d.reset();
		s_pDefaultSampleTextureCube.reset();
		s_pDefaultDepthTexture2dArray.reset();
		s_pDefaultStorageTexture2d.reset();
		s_isInitialized = false;
	}



	// Public methods:
	// Getters:
	// Samplers:
	Sampler* DefaultGpuResources::GetColorSampler()
	{
		return s_pColorSampler.get();
	}
	Sampler* DefaultGpuResources::GetShadowSampler()
	{
		return s_pShadowSampler.get();
	}
	// Materials:
	Material* DefaultGpuResources::GetDefaultPresentMaterial()
	{
		return s_pDefaultPresentMaterial.get();
	}
	Material* DefaultGpuResources::GetDefaultShadowMaterial()
	{
		return s_pDefaultShadowMaterial.get();
	}
	// Compute shaders:
	ComputeShader* DefaultGpuResources::GetGammaCorrectionComputeShader()
	{
		return s_pGammaCorrectionComputeShader.get();
	}
	// Meshes:
	Mesh* DefaultGpuResources::GetDefaultRenderQuad()
	{
		return s_pDefaultRenderQuad.get();
	}
	// Buffers:
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
	SampleTextureCube* DefaultGpuResources::GetDefaultSampleTextureCube()
	{
		return s_pDefaultSampleTextureCube.get();
	}
	DepthTexture2dArray* DefaultGpuResources::GetDefaultDepthTexture2dArray()
	{
		return s_pDefaultDepthTexture2dArray.get();
	}
	StorageTexture2d* DefaultGpuResources::GetDefaultStorageTexture2d()
	{
		return s_pDefaultStorageTexture2d.get();
	}



	// Private methods:
	Mesh DefaultGpuResources::CreateDefaultRenderQuad()
	{
		std::vector<Float3> positions;
		positions.emplace_back(-1.0f, -1.0f, 0.0f);
		positions.emplace_back(-1.0f, 1.0f, 0.0f);
		positions.emplace_back(1.0f, -1.0f, 0.0f);
		positions.emplace_back(1.0f, 1.0f, 0.0f);

		std::vector<Float3> normals;
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);

		std::vector<Float3> tangents;
		tangents.emplace_back(1.0f, 0.0f, 0.0f);
		tangents.emplace_back(1.0f, 0.0f, 0.0f);
		tangents.emplace_back(1.0f, 0.0f, 0.0f);
		tangents.emplace_back(1.0f, 0.0f, 0.0f);

		std::vector<Float4> colors;
		colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
		colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
		colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
		colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);

		std::vector<Float4> uvs;
		uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 1.0f, 0.0f, 0.0f);

		std::vector<Uint3> triangles;
		triangles.emplace_back(Uint3(0, 2, 1));
		triangles.emplace_back(Uint3(1, 2, 3));

		Mesh mesh;
		mesh.UpdateVertexBuffer(positions, &normals, &tangents, &colors, &uvs, emberCommon::VertexMemoryLayout::interleaved);
		mesh.UpdateIndexBuffer(triangles, 4);
		return mesh;
	}
}