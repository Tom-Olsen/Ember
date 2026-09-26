#include "vulkanShadowDrawCall.h"
#include "vulkanMaterial.h"
#include <cassert>



namespace vulkanRendererBackend
{
	ShadowDrawCall::ShadowDrawCall(Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, uint32_t instanceCount)
		: pMesh(pMesh)
		, pMaterial(pMaterial)
		, descriptorSetBindingHandle(descriptorSetBindingHandle)
		, instanceCount(instanceCount)
	{
		assert(pMesh != nullptr);
		assert(pMaterial != nullptr);
		assert(descriptorSetBindingHandle.IsValid());
		assert(pMaterial->GetMaterialPass() == emberCommon::MaterialPass::shadow);
	}
	ShadowDrawCall::~ShadowDrawCall()
	{

	}
}