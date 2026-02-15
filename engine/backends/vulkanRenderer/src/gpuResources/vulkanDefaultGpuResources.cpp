#include "vulkanDefaultGpuResources.h"
#include "emberMath.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanShaderProperties.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageTexture2d.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	// Static members:
	bool DefaultGpuResources::s_isInitialized = false;
	// Materials:
	std::unique_ptr<Material> DefaultGpuResources::s_pDefaultMaterial = nullptr;
	std::unique_ptr<Material> DefaultGpuResources::s_pDefaultPresentMaterial = nullptr;
	// ShaderProperties:
	std::unique_ptr<ShaderProperties> DefaultGpuResources::s_pDefaultShaderProperties = nullptr;
	std::unique_ptr<ShaderProperties> DefaultGpuResources::s_pDefaultPresentShaderProperties = nullptr;
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
	void DefaultGpuResources::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		// Materials:
		std::filesystem::path shadersSrcDirectory = (std::filesystem::path(ENGINE_SHADERS_DIR) / "shaders" / "src").make_preferred();
		s_pDefaultMaterial = std::make_unique<Material>(emberCommon::MaterialType::forwardOpaque, "defaultMaterial", emberCommon::RenderQueue::opaque, shadersSrcDirectory / "vertex" / "default.vert.hlsl", shadersSrcDirectory / "fragment" / "default.frag.hlsl");
		s_pDefaultPresentMaterial = std::make_unique<Material>(emberCommon::MaterialType::present, "presentMaterial", emberCommon::RenderQueue::opaque, shadersSrcDirectory / "vertex" / "present.vert.spv", shadersSrcDirectory / "fragment" / "present.frag.spv");
		// ShaderProperties:
		s_pDefaultShaderProperties = std::make_unique<ShaderProperties>((Shader*)s_pDefaultMaterial.get());
		s_pDefaultPresentShaderProperties = std::make_unique<ShaderProperties>((Shader*)s_pDefaultPresentMaterial.get());
		// Meshes:
		s_pDefaultRenderQuad = std::make_unique<Mesh>();
		CreateDefaultRenderQuad();
		// Buffers:
		s_pDefaultStorageBuffer = std::make_unique<StorageBuffer>(1, sizeof(int), "1x1Dummy");
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
		// Materials:
		s_pDefaultMaterial.reset();
		s_pDefaultPresentMaterial.reset();
		// ShaderProperties:
		s_pDefaultShaderProperties.reset();
		s_pDefaultPresentShaderProperties.reset();
		// Meshes:
		s_pDefaultRenderQuad
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
	// Materials:
	Material* DefaultGpuResources::GetDefaultMaterial()
	{
		return s_pDefaultMaterial.get();
	}
	Material* DefaultGpuResources::GetDefaultPresentMaterial()
	{
		return s_pDefaultPresentMaterial.get();
	}
	// ShaderProperties:
	ShaderProperties* DefaultGpuResources::GetDefaultShaderProperties()
	{
		return s_pDefaultShaderProperties.get();
	}
	ShaderProperties* DefaultGpuResources::GetDefaultPresentShaderProperties()
	{
		return s_pDefaultPresentShaderProperties.get();
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
	void DefaultGpuResources::CreateDefaultRenderQuad()
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

		s_pDefaultRenderQuad->UpdateVertexBuffer(positions, &normals, &tangents, &colors, &uvs, emberCommon::VertexMemoryLayout::interleaved);
		s_pDefaultRenderQuad->UpdateIndexBuffer(triangles, 4);
	}
}