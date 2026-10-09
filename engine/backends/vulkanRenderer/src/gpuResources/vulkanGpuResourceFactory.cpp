#include "vulkanGpuResourceFactory.h"
#include "descriptorSetMacros.h"
#include "vulkanComputeShader.h"
#include "vulkanConvertTextureFormat.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanGarbageCollector.h"
#include "vulkanIndexBuffer.h"
#include "vulkanMaterial.h"
#include "vulkanMaterialShader.h"
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
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	GpuResourceFactory::GpuResourceFactory(uint32_t shadowMapResolution)
		: m_shadowMapResolution(shadowMapResolution)
	{

	}
	GpuResourceFactory::~GpuResourceFactory()
	{

	}



	// Creation:
	emberBackendInterface::IComputeShader* GpuResourceFactory::CreateComputeShader(const emberCommon::ComputeShaderCreateInfo& createInfo)
	{
		return new ComputeShader(createInfo.binaryPath, createInfo.features, createInfo.name);
	}
	emberBackendInterface::IMaterialShader* GpuResourceFactory::CreateMaterialShader(const emberCommon::MaterialShaderCreateInfo& createInfo)
	{
		switch (createInfo.materialPass)
		{
			case emberCommon::MaterialPass::gizmo:
				return new MaterialShader(MaterialShader::CreateGizmoMaterialShader(createInfo.vertexBinaryPath, createInfo.fragmentBinaryPath, createInfo.name));
			case emberCommon::MaterialPass::outline:
				return new MaterialShader(MaterialShader::CreateOutlineMaterialShader(createInfo.vertexBinaryPath, createInfo.fragmentBinaryPath, createInfo.name));
			case emberCommon::MaterialPass::shadow:
				return new MaterialShader(MaterialShader::CreateShadowMaterialShader(m_shadowMapResolution, createInfo.vertexBinaryPath, createInfo.name));
			case emberCommon::MaterialPass::deferredGeometry:
				return new MaterialShader(MaterialShader::CreateDeferredGeometryMaterialShader(createInfo.vertexBinaryPath, createInfo.fragmentBinaryPath, createInfo.name));
			case emberCommon::MaterialPass::deferredLighting:
				return new MaterialShader(MaterialShader::CreateDeferredLightingMaterialShader(createInfo.vertexBinaryPath, createInfo.fragmentBinaryPath, createInfo.name));
			case emberCommon::MaterialPass::forward:
				return new MaterialShader(MaterialShader::CreateForwardMaterialShader(createInfo.vertexBinaryPath, createInfo.fragmentBinaryPath, createInfo.name));
			case emberCommon::MaterialPass::present:
				return new MaterialShader(MaterialShader::CreatePresentMaterialShader(createInfo.vertexBinaryPath, createInfo.fragmentBinaryPath, createInfo.name));
			default:
				throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterialShader(...) failed. Unsupported material pass: " + std::string(emberCommon::MaterialPassToString(createInfo.materialPass)));
		}
	}
	emberBackendInterface::IMaterial* GpuResourceFactory::CreateMaterial(emberBackendInterface::IMaterialShader* pMaterialShader, const emberCommon::MaterialCreateInfo& createInfo)
	{
		MaterialShader* pVulkanMaterialShader = static_cast<MaterialShader*>(pMaterialShader);
		if (pVulkanMaterialShader == nullptr)
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. pMaterialShader is nullptr.");
		if (pVulkanMaterialShader->GetMaterialPass() != createInfo.materialPass)
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Material and material shader passes do not match.");

		switch (createInfo.materialPass)
		{
			case emberCommon::MaterialPass::gizmo:
			{
				const emberCommon::GizmoRenderMode* pGizmoRenderMode = std::get_if<emberCommon::GizmoRenderMode>(&createInfo.renderMode);
				if (pGizmoRenderMode == nullptr || *pGizmoRenderMode == emberCommon::GizmoRenderMode::count)
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Gizmo render mode is invalid.");
				return new Material(Material::CreateGizmo(pVulkanMaterialShader, *pGizmoRenderMode, createInfo.name));
			}
			case emberCommon::MaterialPass::outline:
				if (!std::holds_alternative<std::monostate>(createInfo.renderMode))
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Outline materials do not use a render mode.");
				return new Material(Material::CreateOutline(pVulkanMaterialShader, createInfo.name));
			case emberCommon::MaterialPass::shadow:
				if (!std::holds_alternative<std::monostate>(createInfo.renderMode))
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Shadow materials do not use a render mode.");
				return new Material(Material::CreateShadow(pVulkanMaterialShader, createInfo.name));
			case emberCommon::MaterialPass::deferredGeometry:
				if (!std::holds_alternative<std::monostate>(createInfo.renderMode))
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Deferred geometry materials do not use a render mode.");
				return new Material(Material::CreateDeferredGeometry(pVulkanMaterialShader, createInfo.name));
			case emberCommon::MaterialPass::deferredLighting:
				if (!std::holds_alternative<std::monostate>(createInfo.renderMode))
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Deferred lighting materials do not use a render mode.");
				return new Material(Material::CreateDeferredLighting(pVulkanMaterialShader, createInfo.name));
			case emberCommon::MaterialPass::forward:
			{
				const emberCommon::ForwardRenderMode* pForwardRenderMode = std::get_if<emberCommon::ForwardRenderMode>(&createInfo.renderMode);
				if (pForwardRenderMode == nullptr || *pForwardRenderMode == emberCommon::ForwardRenderMode::count)
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Forward render mode is invalid.");
				return new Material(Material::CreateForward(pVulkanMaterialShader, *pForwardRenderMode, createInfo.name));
			}
			case emberCommon::MaterialPass::present:
				if (!std::holds_alternative<std::monostate>(createInfo.renderMode))
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Present materials do not use a render mode.");
				return new Material(Material::CreatePresent(pVulkanMaterialShader, createInfo.name));
			default:
				throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateMaterial(...) failed. Unsupported material pass: " + std::string(emberCommon::MaterialPassToString(createInfo.materialPass)));
		}
	}
	emberBackendInterface::IMaterial* GpuResourceFactory::CloneMaterial(emberBackendInterface::IMaterial* pSourceMaterial, const emberCommon::MaterialCloneInfo& cloneInfo)
	{
		Material* pVulkanSourceMaterial = static_cast<Material*>(pSourceMaterial);
		if (pVulkanSourceMaterial == nullptr)
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CloneMaterial(...) failed. pSourceMaterial is nullptr.");

		bool useDefaultBindings = cloneInfo.bindingCloneMode == emberCommon::MaterialBindingCloneMode::defaultBindings;
		switch (pVulkanSourceMaterial->GetMaterialPass())
		{
			case emberCommon::MaterialPass::gizmo:
				return useDefaultBindings
					? new Material(Material::CloneGizmoWithDefaultBindings(*pVulkanSourceMaterial, cloneInfo.name))
					: new Material(Material::CloneGizmo(*pVulkanSourceMaterial, cloneInfo.name));
			case emberCommon::MaterialPass::outline:
				if (useDefaultBindings)
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CloneMaterial(...) failed. Outline materials do not support cloning with default bindings.");
				return new Material(Material::CloneOutline(*pVulkanSourceMaterial, cloneInfo.name));
			case emberCommon::MaterialPass::shadow:
				if (useDefaultBindings)
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CloneMaterial(...) failed. Shadow materials do not support cloning with default bindings.");
				return new Material(Material::CloneShadow(*pVulkanSourceMaterial, cloneInfo.name));
			case emberCommon::MaterialPass::deferredGeometry:
				return useDefaultBindings
					? new Material(Material::CloneDeferredGeometryWithDefaultBindings(*pVulkanSourceMaterial, cloneInfo.name))
					: new Material(Material::CloneDeferredGeometry(*pVulkanSourceMaterial, cloneInfo.name));
			case emberCommon::MaterialPass::deferredLighting:
				if (useDefaultBindings)
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CloneMaterial(...) failed. Deferred lighting materials do not support cloning with default bindings.");
				return new Material(Material::CloneDeferredLighting(*pVulkanSourceMaterial, cloneInfo.name));
			case emberCommon::MaterialPass::forward:
				return useDefaultBindings
					? new Material(Material::CloneForwardWithDefaultBindings(*pVulkanSourceMaterial, cloneInfo.name))
					: new Material(Material::CloneForward(*pVulkanSourceMaterial, cloneInfo.name));
			case emberCommon::MaterialPass::present:
				if (useDefaultBindings)
					throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CloneMaterial(...) failed. Present materials do not support cloning with default bindings.");
				return new Material(Material::ClonePresent(*pVulkanSourceMaterial, cloneInfo.name));
			default:
				throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CloneMaterial(...) failed. Unsupported material pass.");
		}
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
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTexture2d(vulkanFormat, width, height, data, imageCountMode);
			break;
		case emberCommon::TextureUsage::storage:
			pITexture = new StorageTexture2d(vulkanFormat, width, height, data, imageCountMode);
			break;
		case emberCommon::TextureUsage::storageSample:
			pITexture = new StorageSampleTexture2d(vulkanFormat, width, height, data, imageCountMode);
			break;
		case emberCommon::TextureUsage::renderTarget:
			pITexture = new RenderTexture2d(vulkanFormat, width, height, imageCountMode);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateTexture2d(...) failed. Invalid TextureUsage type: " + std::string(emberCommon::TextureUsageToString(usage)));
		}
		return pITexture;
	}
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTexture3d(vulkanFormat, width, height, depth, data, imageCountMode);
			break;
		case emberCommon::TextureUsage::storage:
			pITexture = new StorageTexture3d(vulkanFormat, width, height, depth, data, imageCountMode);
			break;
		case emberCommon::TextureUsage::storageSample:
			pITexture = new StorageSampleTexture3d(vulkanFormat, width, height, depth, data, imageCountMode);
			break;
		default:
			throw std::runtime_error("vulkanRendererBackend::GpuResourceFactory::CreateTexture3d(...) failed. Unsupported TextureUsage type: " + std::string(emberCommon::TextureUsageToString(usage)));
		}
		return pITexture;
	}
	emberBackendInterface::ITexture* GpuResourceFactory::CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode, void* data)
	{
		VkFormat vulkanFormat = TextureFormatCommonToVulkan(format);
		emberBackendInterface::ITexture* pITexture = nullptr;
		switch (usage)
		{
		case emberCommon::TextureUsage::sample:
			pITexture = new SampleTextureCube(vulkanFormat, width, height, data, imageCountMode);
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



	// Retirement:
	void GpuResourceFactory::RetireComputeShader(emberBackendInterface::IComputeShader* pComputeShader)
	{
		ComputeShader* pVulkanComputeShader = static_cast<ComputeShader*>(pComputeShader);
		if (pVulkanComputeShader == nullptr)
			return;

		GarbageCollector::RecordPendingGarbage([pVulkanComputeShader]()
		{
			if (pVulkanComputeShader->HasPendingUse())
				return false;
			delete pVulkanComputeShader;
			return true;
		});
	}
	void GpuResourceFactory::RetireMaterial(emberBackendInterface::IMaterial* pMaterial)
	{
		Material* pVulkanMaterial = static_cast<Material*>(pMaterial);
		if (pVulkanMaterial == nullptr)
			return;

		GarbageCollector::RecordFrameGarbage([pVulkanMaterial]()
		{
			delete pVulkanMaterial;
		});
	}
	void GpuResourceFactory::RetireMaterialShader(emberBackendInterface::IMaterialShader* pMaterialShader)
	{
		MaterialShader* pVulkanMaterialShader = static_cast<MaterialShader*>(pMaterialShader);
		if (pVulkanMaterialShader == nullptr)
			return;

		GarbageCollector::RecordFrameGarbage([pVulkanMaterialShader]()
		{
			delete pVulkanMaterialShader;
		});
	}
	void GpuResourceFactory::RetireTexture(emberBackendInterface::ITexture* pITexture)
	{
		if (pITexture == nullptr)
			return;

		GarbageCollector::RecordFrameGarbage([pITexture]()
		{
			delete pITexture;
		});
	}
}