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
	class IMaterialShader;
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

		// Creation:
		emberBackendInterface::IComputeShader* CreateComputeShader(const emberCommon::ComputeShaderCreateInfo& createInfo) override;
		emberBackendInterface::IMaterialShader* CreateMaterialShader(const emberCommon::MaterialShaderCreateInfo& createInfo) override;
		emberBackendInterface::IMaterial* CreateMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const emberCommon::MaterialCreateInfo& createInfo) override;
		emberBackendInterface::IMaterial* CloneMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const emberCommon::MaterialCloneInfo& cloneInfo) override;
		emberBackendInterface::IBuffer* CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage) override;
		//emberBackendInterface::ITexture* CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) override;
		emberBackendInterface::ITexture* CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) override;
		emberBackendInterface::ITexture* CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) override;
		emberBackendInterface::ITexture* CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data) override;
		emberBackendInterface::IMesh* CreateMesh() override;
		emberBackendInterface::IDescriptorSetBinding* CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial) override;

		// Retirement:
		void RetireComputeShader(emberBackendInterface::IComputeShader* pComputeShader) override;
		void RetireMaterial(emberBackendInterface::IMaterial* pMaterial) override;
		void RetireMaterialShader(emberBackendInterface::IMaterialShader* pMaterialShader) override;
	};
}