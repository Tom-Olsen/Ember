#pragma once
#include "commonBufferUsage.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include <cstdint>



namespace emberBackendInterface
{
	// Forward declarations:
	class IBuffer;
	class IComputeShaderManager;
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

		// Gpu resource factories:
		virtual IComputeShaderManager* GetComputeShaderManager() = 0;
		virtual IMaterialManager* GetMaterialManager() = 0;
		virtual IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage) = 0;
		//virtual ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
		virtual ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
		virtual ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
		virtual ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) = 0;
		virtual IMesh* CreateMesh() = 0;
		virtual IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(IMaterial* pIMaterial) = 0;
	};
}