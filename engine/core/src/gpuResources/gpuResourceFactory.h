#pragma once
#include "commonBufferUsage.h"
#include "commonComputeShaderCreateInfo.h"
#include "commonMaterialCloneInfo.h"
#include "commonMaterialCreateInfo.h"
#include "commonMaterialShaderCreateInfo.h"
#include "commonTextureFormat.h"
#include "commonTextureImageCountMode.h"
#include "commonTextureUsage.h"
#include "emberCoreExport.h"
#include <cstdint>



// Forward declarations:
namespace emberBackendInterface
{
	class IBuffer;
	class IComputeShader;
	class IDescriptorSetBinding;
	class IGpuResourceFactory;
	class IMaterial;
	class IMaterialShader;
	class IMesh;
	class ITexture;
}



namespace emberCore
{
	// Forward declarations:
	class Buffer;
	class CallProperties;
	class ComputeShaderManager;
	class Core;
	class MaterialManager;
	class MaterialShaderManager;
	class Mesh;
	class Renderer;
	class Texture2d;
	class Texture3d;
	class TextureCube;



	class EMBER_CORE_API GpuResourceFactory
	{
		// Friends:
		friend class Buffer;
		friend class CallProperties;
		friend class ComputeShaderManager;
		friend class Core;
		friend class MaterialManager;
		friend class MaterialShaderManager;
		friend class Mesh;
		friend class Renderer;
		friend class Texture2d;
		friend class Texture3d;
		friend class TextureCube;

	private: // Members:
		static bool s_isInitialized;
		static emberBackendInterface::IGpuResourceFactory* s_pIGpuResourceFactory;

	private: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::IGpuResourceFactory* pGpuResourceFactory);
		static void Clear();

		// Creation:
		static emberBackendInterface::IComputeShader* CreateComputeShader(const emberCommon::ComputeShaderCreateInfo& createInfo);
		static emberBackendInterface::IMaterialShader* CreateMaterialShader(const emberCommon::MaterialShaderCreateInfo& createInfo);
		static emberBackendInterface::IMaterial* CreateMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const emberCommon::MaterialCreateInfo& createInfo);
		static emberBackendInterface::IMaterial* CloneMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const emberCommon::MaterialCloneInfo& cloneInfo);
		static emberBackendInterface::IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage);
		static emberBackendInterface::ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data);
		static emberBackendInterface::ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data);
		static emberBackendInterface::ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data);
		static emberBackendInterface::IMesh* CreateMesh();
		static emberBackendInterface::IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pMaterial);

		// Retirement:
		static void RetireComputeShader(emberBackendInterface::IComputeShader* pComputeShader);
		static void RetireMaterial(emberBackendInterface::IMaterial* pMaterial);
		static void RetireMaterialShader(emberBackendInterface::IMaterialShader* pMaterialShader);

		// Delete all constructors:
		GpuResourceFactory() = delete;
		GpuResourceFactory(const GpuResourceFactory&) = delete;
		GpuResourceFactory& operator=(const GpuResourceFactory&) = delete;
		GpuResourceFactory(GpuResourceFactory&&) = delete;
		GpuResourceFactory& operator=(GpuResourceFactory&&) = delete;
		~GpuResourceFactory() = delete;
	};
}