#include "vulkanGpuResourceFactory.h"
#include "descriptorSetMacros.h"
#include "vulkanConvertTextureFormat.h"
#include "vulkanComputeShaderManager.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanIndexBuffer.h"
#include "vulkanMaterial.h"
#include "vulkanMaterialManager.h"
#include "vulkanMaterialShaderManager.h"
#include "vulkanMesh.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSampleTexture3d.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageSampleTexture2d.h"
#include "vulkanStorageSampleTexture3d.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanStorageTexture3d.h"
#include "vulkanVertexBuffer.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	GpuResourceFactory::GpuResourceFactory(uint32_t shadowMapResolution)
	{
		m_pMaterialShaderManager = std::make_unique<MaterialShaderManager>(shadowMapResolution);
		m_pMaterialManager = std::make_unique<MaterialManager>(m_pMaterialShaderManager.get());
		m_pComputeShaderManager = std::make_unique<ComputeShaderManager>();
	}
	GpuResourceFactory::~GpuResourceFactory()
	{

	}



	// Gpu resource factories:
	emberBackendInterface::IMaterialManager* GpuResourceFactory::GetMaterialManager()
	{
		return m_pMaterialManager.get();
	}
	emberBackendInterface::IComputeShaderManager* GpuResourceFactory::GetComputeShaderManager()
	{
		return m_pComputeShaderManager.get();
	}
	emberBackendInterface::IBuffer* GpuResourceFactory::CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage)
	{
		emberBackendInterface::IBuffer* pIBuffer = nullptr;
		switch (usage)
		{
		case emberCommon::BufferUsage::index:
			pIBuffer = new IndexBuffer(count, elementSize);
			break;
		case emberCommon::BufferUsage::storage:
			pIBuffer = new StorageBuffer(count, elementSize);
			break;
		case emberCommon::BufferUsage::vertex:
			pIBuffer = new VertexBuffer(count, elementSize);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateBuffer(...) failed. Unknown invalid BufferUsage type: " + std::string(emberCommon::BufferUsageToString(usage)));
		}
		return pIBuffer;
	}
	//emberBackendInterface::ITexture* GpuResourceFactory::CreateTexture1d(int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage)
	//{
	//
	//}
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTexture2d(vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::storage:
			pITexture = new StorageTexture2d(vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::storageSample:
			pITexture = new StorageSampleTexture2d(vulkanFormat, width, height, data);
			break;
		case emberCommon::TextureUsage::renderTarget:
			pITexture = new RenderTexture2d(vulkanFormat, width, height);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateTexture2d(...) failed. Invalid TextureUsage type: " + std::string(emberCommon::TextureUsageToString(usage)));
		}
		return pITexture;
	}
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTexture3d(vulkanFormat, width, height, depth, data);
			break;
		case emberCommon::TextureUsage::storage:
			pITexture = new StorageTexture3d(vulkanFormat, width, height, depth, data);
			break;
		case emberCommon::TextureUsage::storageSample:
			pITexture = new StorageSampleTexture3d(vulkanFormat, width, height, depth, data);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateTexture3d(...) failed. Unsupported TextureUsage type: " + std::string(emberCommon::TextureUsageToString(usage)));
		}
		return pITexture;
	}
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTextureCube(vulkanFormat, width, height, data);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateTextureCube(...) failed. Invalid TextureUsage type: " + std::string(emberCommon::TextureUsageToString(usage)));
		}
		return pITexture;
	}
	emberBackendInterface::IMesh* GpuResourceFactory::CreateMesh()
	{
		return new Mesh();
	}
	emberBackendInterface::IDescriptorSetBinding* GpuResourceFactory::CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial)
	{
		Material* pMaterial = static_cast<Material*>(pIMaterial);
		Shader* pShader = pMaterial->GetShader();
		return new DescriptorSetBinding(pShader, CALL_SET_INDEX, pMaterial->GetDebugName());
	}
}