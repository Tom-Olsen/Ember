#pragma once
#include "commonBufferUsage.h"
#include "commonComputeShaderCreateInfo.h"
#include "commonMaterialCloneInfo.h"
#include "commonMaterialCreateInfo.h"
#include "commonMaterialShaderCreateInfo.h"
#include "commonTextureFormat.h"
#include "commonTextureImageCountMode.h"
#include "commonTextureUsage.h"
#include <cstdint>



namespace emberBackendInterface
{
	// Forward declarations:
	class IBuffer;
	class IComputeShader;
	class IDescriptorSetBinding;
	class IMaterial;
	class IMaterialShader;
	class IMesh;
	class ITexture;


	
	class IGpuResourceFactory
	{
	public: // Methods:
		// Virtual destructor for v-table:
	    virtual ~IGpuResourceFactory() = default;

		// Creation:
		virtual IComputeShader* CreateComputeShader(const emberCommon::ComputeShaderCreateInfo& createInfo) = 0;
		virtual IMaterialShader* CreateMaterialShader(const emberCommon::MaterialShaderCreateInfo& createInfo) = 0;
		virtual IMaterial* CreateMaterial(IMaterialShader* pMaterialShader, const emberCommon::MaterialCreateInfo& createInfo) = 0;
		virtual IMaterial* CloneMaterial(IMaterial* pSourceMaterial, const emberCommon::MaterialCloneInfo& cloneInfo) = 0;
		virtual IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage) = 0;
		//virtual ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) = 0;
		virtual ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) = 0;
		virtual ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) = 0;
		virtual ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) = 0;
		virtual IMesh* CreateMesh() = 0;
		virtual IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(IMaterial* pIMaterial) = 0;

		// Retirement:
		virtual void RetireComputeShader(IComputeShader* pComputeShader) = 0;
		virtual void RetireMaterial(IMaterial* pMaterial) = 0;
		virtual void RetireMaterialShader(IMaterialShader* pMaterialShader) = 0;
		virtual void RetireTexture(ITexture* pITexture) = 0;
	};
}