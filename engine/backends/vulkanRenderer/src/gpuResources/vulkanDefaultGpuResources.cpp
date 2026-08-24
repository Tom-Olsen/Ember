#include "vulkanDefaultGpuResources.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "iMaterial.h"
#include "iRenderer.h"
#include "vulkanColorSampler.h"
#include "vulkanComputeShader.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
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
	bool DefaultGpuResources::s_isInitialized = false;
	// Samplers:
	std::unique_ptr<Sampler> DefaultGpuResources::s_pColorSampler = nullptr;
	std::unique_ptr<Sampler> DefaultGpuResources::s_pColorSamplerClampEdge = nullptr;
	std::unique_ptr<Sampler> DefaultGpuResources::s_pShadowSampler = nullptr;
	// Materials:
	Material* DefaultGpuResources::s_pDefaultOutlineMaterial = nullptr;
	Material* DefaultGpuResources::s_pDefaultShadowMaterial = nullptr;
	Material* DefaultGpuResources::s_pDefaultPresentMaterial = nullptr;
	// Compute shaders:
	std::unique_ptr<ComputeShader> DefaultGpuResources::s_pGammaCorrectionComputeShader = nullptr;
	std::unique_ptr<ComputeShader> DefaultGpuResources::s_pOutlineComputeShader = nullptr;
	// Meshes:
	std::unique_ptr<Mesh> DefaultGpuResources::s_pDefaultRenderQuad = nullptr;
	// Buffers:
	std::unique_ptr<StorageBuffer> DefaultGpuResources::s_pDefaultStorageBuffer = nullptr;
	// Textures:
	std::unique_ptr<SampleTexture2d> DefaultGpuResources::s_pDefaultSampleTexture2d = nullptr;
	std::unique_ptr<SampleTexture2d> DefaultGpuResources::s_pDefaultNormalMap = nullptr;
	std::unique_ptr<SampleTexture3d> DefaultGpuResources::s_pDefaultSampleTexture3d = nullptr;
	std::unique_ptr<SampleTextureCube> DefaultGpuResources::s_pDefaultSampleTextureCube = nullptr;
	std::unique_ptr<DepthTexture2dArray> DefaultGpuResources::s_pDefaultDepthTexture2dArray = nullptr;
	std::unique_ptr<StorageTexture2d> DefaultGpuResources::s_pDefaultStorageTexture2d = nullptr;
	std::unique_ptr<StorageTexture3d> DefaultGpuResources::s_pDefaultStorageTexture3d = nullptr;



	// Initialization/Cleanup:
	void DefaultGpuResources::InitSamplers()
	{
		if (!s_pColorSampler)
        {
			ColorSampler::Settings settings;
			settings.name = "Sampler_Color";
            s_pColorSampler = std::make_unique<ColorSampler>(ColorSampler::Settings{});
        }
		if (!s_pColorSamplerClampEdge)
		{
			ColorSampler::Settings settings;
			settings.name = "Sampler_ColorClampEdge";
			settings.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			s_pColorSamplerClampEdge = std::make_unique<ColorSampler>(settings);
		}
		if (!s_pShadowSampler)
			s_pShadowSampler = std::make_unique<ShadowSampler>("Sampler_Shadow");
	}
	void DefaultGpuResources::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
		std::filesystem::path shadersBinDirectory = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();

		// Samplers:
		InitSamplers();
		// Buffers:
		s_pDefaultStorageBuffer = std::make_unique<StorageBuffer>(1, sizeof(int));
		// Textures:
		std::array<unsigned char, 4> whitePixel = { 255, 255, 255, 255 };
		std::array<unsigned char, 4> normalPixel = { 128, 128, 255, 255 };
		s_pDefaultSampleTexture2d = std::make_unique<SampleTexture2d>(VK_FORMAT_R8G8B8A8_UNORM, 1, 1, whitePixel.data());
		s_pDefaultNormalMap = std::make_unique<SampleTexture2d>(VK_FORMAT_R8G8B8A8_UNORM, 1, 1, normalPixel.data());
		s_pDefaultSampleTexture3d = std::make_unique<SampleTexture3d>(VK_FORMAT_R8G8B8A8_UNORM, 1, 1, 1, whitePixel.data());
		std::array<Float4, 6> whiteFaces = { Float4::white, Float4::white, Float4::white, Float4::white, Float4::white, Float4::white };
		s_pDefaultSampleTextureCube = std::make_unique<SampleTextureCube>(VK_FORMAT_R32G32B32A32_SFLOAT, 1, 1, whiteFaces.data());
		s_pDefaultDepthTexture2dArray = std::make_unique<DepthTexture2dArray>(VK_FORMAT_D32_SFLOAT, 2, 1, 1);
		s_pDefaultStorageTexture2d = std::make_unique<StorageTexture2d>(VK_FORMAT_R32G32B32A32_SFLOAT, 1, 1, (void*)&Float4::one);
		s_pDefaultStorageTexture3d = std::make_unique<StorageTexture3d>(VK_FORMAT_R32G32B32A32_SFLOAT, 1, 1, 1, (void*)&Float4::one);
		// Compute shaders:
		s_pGammaCorrectionComputeShader = std::make_unique<ComputeShader>(shadersBinDirectory / "gammaCorrection.comp.spv", "gammaCorrectionComputeShader");
		s_pOutlineComputeShader = std::make_unique<ComputeShader>(shadersBinDirectory / "outlineComposite.comp.spv", "outlineComputeShader");
		// Meshes:
		s_pDefaultRenderQuad = std::make_unique<Mesh>(CreateDefaultRenderQuad());
	}
	void DefaultGpuResources::Clear()
	{
		// Samplers:
		s_pColorSampler.reset();
		s_pColorSamplerClampEdge.reset();
		s_pShadowSampler.reset();
		// Materials:
		ClearDefaultMaterials();
		// Compute shaders:
		s_pGammaCorrectionComputeShader.reset();
		s_pOutlineComputeShader.reset();
		// Meshes:
		s_pDefaultRenderQuad.reset();
		// Buffers:
		s_pDefaultStorageBuffer.reset();
		// Textures:
		s_pDefaultSampleTexture2d.reset();
		s_pDefaultNormalMap.reset();
		s_pDefaultSampleTexture3d.reset();
		s_pDefaultSampleTextureCube.reset();
		s_pDefaultDepthTexture2dArray.reset();
		s_pDefaultStorageTexture2d.reset();
		s_pDefaultStorageTexture3d.reset();
		s_isInitialized = false;
	}



	// Public methods:
	// Set/Clear default materials:
	void DefaultGpuResources::SetDefaultMaterials(emberBackendInterface::IMaterial* pOutlineMaterial, emberBackendInterface::IMaterial* pDefaultShadowMaterial, emberBackendInterface::IMaterial* pPresentMaterial)
	{
		if (pOutlineMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::SetDefaultMaterials(...) failed. Outline material is null.");
		if (pDefaultShadowMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::SetDefaultMaterials(...) failed. Default shadow material is null.");
		if (pPresentMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::SetDefaultMaterials(...) failed. Present material is null.");
		if (pOutlineMaterial->GetMaterialPass() != emberCommon::MaterialPass::outline)
			throw std::runtime_error("DefaultGpuResources::SetDefaultMaterials(...) failed. Outline material has wrong material pass.");
		if (pDefaultShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("DefaultGpuResources::SetDefaultMaterials(...) failed. Default shadow material has wrong material pass.");
		if (pPresentMaterial->GetMaterialPass() != emberCommon::MaterialPass::present)
			throw std::runtime_error("DefaultGpuResources::SetDefaultMaterials(...) failed. Present material has wrong material pass.");

		s_pDefaultOutlineMaterial = static_cast<Material*>(pOutlineMaterial);
		s_pDefaultShadowMaterial = static_cast<Material*>(pDefaultShadowMaterial);
		s_pDefaultPresentMaterial = static_cast<Material*>(pPresentMaterial);
	}
	void DefaultGpuResources::ClearDefaultMaterials()
	{
		s_pDefaultOutlineMaterial = nullptr;
		s_pDefaultShadowMaterial = nullptr;
		s_pDefaultPresentMaterial = nullptr;
	}
	// Samplers:
	Sampler* DefaultGpuResources::GetColorSampler()
	{
		return s_pColorSampler.get();
	}
	Sampler* DefaultGpuResources::GetColorSamplerClampEdge()
	{
		return s_pColorSamplerClampEdge.get();
	}
	Sampler* DefaultGpuResources::GetShadowSampler()
	{
		return s_pShadowSampler.get();
	}
	// Materials:
	Material* DefaultGpuResources::GetDefaultOutlineMaterial()
	{
		if (s_pDefaultOutlineMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetDefaultOutlineMaterial() failed. Default outline material is not set.");
		return s_pDefaultOutlineMaterial;
	}
	Material* DefaultGpuResources::GetDefaultShadowMaterial()
	{
		if (s_pDefaultShadowMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetDefaultShadowMaterial() failed. Default shadow material is not set.");
		return s_pDefaultShadowMaterial;
	}
	Material* DefaultGpuResources::GetDefaultPresentMaterial()
	{
		if (s_pDefaultPresentMaterial == nullptr)
			throw std::runtime_error("DefaultGpuResources::GetDefaultPresentMaterial() failed. Default present material is not set.");
		return s_pDefaultPresentMaterial;
	}
	// Compute shaders:
	ComputeShader* DefaultGpuResources::GetGammaCorrectionComputeShader()
	{
		return s_pGammaCorrectionComputeShader.get();
	}
	ComputeShader* DefaultGpuResources::GetOutlineComputeShader()
	{
		return s_pOutlineComputeShader.get();
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
	SampleTexture2d* DefaultGpuResources::GetDefaultNormalMap()
	{
		return s_pDefaultNormalMap.get();
	}
	SampleTexture3d* DefaultGpuResources::GetDefaultSampleTexture3d()
	{
		return s_pDefaultSampleTexture3d.get();
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
	StorageTexture3d* DefaultGpuResources::GetDefaultStorageTexture3d()
	{
		return s_pDefaultStorageTexture3d.get();
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