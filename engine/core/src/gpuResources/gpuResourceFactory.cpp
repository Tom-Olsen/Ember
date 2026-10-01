#include "gpuResourceFactory.h"
#include "iGpuResourceFactory.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	bool GpuResourceFactory::s_isInitialized = false;
	emberBackendInterface::IGpuResourceFactory* GpuResourceFactory::s_pIGpuResourceFactory = nullptr;



	// Private methods:
	// Initialization/Cleanup:
	void GpuResourceFactory::Init(emberBackendInterface::IGpuResourceFactory* pGpuResourceFactory)
	{
		if (s_isInitialized)
			return;
		if (pGpuResourceFactory == nullptr)
			throw std::runtime_error("GpuResourceFactory::Init(...) failed. pGpuResourceFactory is nullptr.");
		s_pIGpuResourceFactory = pGpuResourceFactory;
		s_isInitialized = true;
	}
	void GpuResourceFactory::Clear()
	{
		s_pIGpuResourceFactory = nullptr;
		s_isInitialized = false;
	}



	// Creation:
	emberBackendInterface::IMaterialManager* GpuResourceFactory::GetMaterialManager()
	{
		return s_pIGpuResourceFactory->GetMaterialManager();
	}
	emberBackendInterface::IComputeShader* GpuResourceFactory::CreateComputeShader(const emberCommon::ComputeShaderCreateInfo& createInfo)
	{
		return s_pIGpuResourceFactory->CreateComputeShader(createInfo);
	}
	emberBackendInterface::IBuffer* GpuResourceFactory::CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage)
	{
		return s_pIGpuResourceFactory->CreateBuffer(count, elementSize, usage);
	}
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data)
	{
		return s_pIGpuResourceFactory->CreateTexture2d(width, height, format, usage, imageCountMode, data);
	}
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data)
	{
		return s_pIGpuResourceFactory->CreateTexture3d(width, height, depth, format, usage, imageCountMode, data);
	}
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data)
	{
		return s_pIGpuResourceFactory->CreateTextureCube(width, height, format, usage, imageCountMode, data);
	}
	emberBackendInterface::IMesh* GpuResourceFactory::CreateMesh()
	{
		return s_pIGpuResourceFactory->CreateMesh();
	}
	emberBackendInterface::IDescriptorSetBinding* GpuResourceFactory::CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pMaterial)
	{
		return s_pIGpuResourceFactory->CreateDrawCallDescriptorSetBinding(pMaterial);
	}



	// Retirement:
	void GpuResourceFactory::RetireComputeShader(emberBackendInterface::IComputeShader* pComputeShader)
	{
		s_pIGpuResourceFactory->RetireComputeShader(pComputeShader);
	}
}