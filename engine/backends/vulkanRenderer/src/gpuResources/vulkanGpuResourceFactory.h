#pragma once
#include "iGpuResourceFactory.h"
#include "vulkanRendererExport.h"
#include <cstdint>
#include <memory>



// Forward declarations:
namespace emberBackendInterface
{
	class IBuffer;
	class IComputeShaderManager;
	class IDescriptorSetBinding;
	class IMaterial;
	class IMaterialManager;
	class IMesh;
	class ITexture;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShaderManager;
	class MaterialManager;
	class MaterialShaderManager;



	class VULKAN_RENDERER_API GpuResourceFactory : public emberBackendInterface::IGpuResourceFactory
	{
	private: // Members:
		std::unique_ptr<MaterialShaderManager> m_pMaterialShaderManager;
		std::unique_ptr<MaterialManager> m_pMaterialManager;
		std::unique_ptr<ComputeShaderManager> m_pComputeShaderManager;



	public: // Methods:
		// Constructor/Destructor:
		GpuResourceFactory(uint32_t shadowMapResolution);
		~GpuResourceFactory();

		// Gpu resource factories:
		emberBackendInterface::IMaterialManager* GetMaterialManager() override;
		emberBackendInterface::IComputeShaderManager* GetComputeShaderManager() override;
		emberBackendInterface::IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage) override;
		//emberBackendInterface::ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::IMesh* CreateMesh() override;
		emberBackendInterface::IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial) override;
	};
}