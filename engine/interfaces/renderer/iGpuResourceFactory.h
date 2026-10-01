#pragma once
#include "commonBufferUsage.h"
#include "commonComputeShaderCreateInfo.h"
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
	class IMaterialManager;
	class IMesh;
	class ITexture;


	
	class IGpuResourceFactory
	{
	public: // Methods:
		// Virtual destructor for v-table:
	    virtual ~IGpuResourceFactory() = default;

		// Creation:
		virtual IMaterialManager* GetMaterialManager() = 0;
		virtual IComputeShader* CreateComputeShader(const emberCommon::ComputeShaderCreateInfo& createInfo) = 0;
		virtual IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage) = 0;
		//virtual ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) = 0;
		virtual ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) = 0;
		virtual ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) = 0;
		virtual ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) = 0;
		virtual IMesh* CreateMesh() = 0;
		virtual IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(IMaterial* pIMaterial) = 0;

		// Retirement:
		virtual void RetireComputeShader(IComputeShader* pComputeShader) = 0;
	};
}