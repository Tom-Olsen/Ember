#pragma once
#include "iGpuResourceFactory.h"
#include "vulkanRendererExport.h"
#include <cstdint>



// Forward declarations:
namespace emberBackendInterface
{
	class IBuffer;
	class IComputeShader;
	class IDescriptorSetBinding;
	class IMaterial;
	class IMaterialManager;
	class IMaterialShaderManager;
	class IMesh;
	class ITexture;
}



namespace vulkanRendererBackend
{
	class VULKAN_RENDERER_API GpuResourceFactory : public emberBackendInterface::IGpuResourceFactory
	{
	private: // Members:
		uint32_t m_shadowMapResolution;



	public: // Methods:
		// Constructor/Destructor:
		GpuResourceFactory(uint32_t shadowMapResolution);
		~GpuResourceFactory();

		// Gpu resource factories:
		emberBackendInterface::IMaterialShaderManager* CreateMaterialShaderManager() override;
		emberBackendInterface::IMaterialManager* CreateMaterialManager(emberBackendInterface::IMaterialShaderManager* pIMaterialShaderManager) override;
		emberBackendInterface::IComputeShader* CreateComputeShader(const std::filesystem::path& computeSpv, const std::string& debugName) override;
		emberBackendInterface::IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage) override;
		//emberBackendInterface::ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data) override;
		emberBackendInterface::IMesh* CreateMesh() override;
		emberBackendInterface::IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial) override;
	};
}