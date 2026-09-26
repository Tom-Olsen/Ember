#include "vulkanDeferredDrawCall.h"
#include "commonCullMode.h"
#include "vulkanMaterial.h"
#include <cassert>



namespace vulkanRendererBackend
{
	DeferredDrawCall::DeferredDrawCall(Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount)
		: pMesh(pMesh)
		, pMaterial(pMaterial)
		, descriptorSetBindingHandle(descriptorSetBindingHandle)
		, cullMode(cullMode)
		, receiveShadows(receiveShadows)
		, instanceCount(instanceCount)
	{
		assert(pMesh != nullptr);
		assert(pMaterial != nullptr);
		assert(descriptorSetBindingHandle.IsValid());
		assert(cullMode != emberCommon::CullMode::count);
		assert(cullMode != emberCommon::CullMode::materialDefault);
		assert(pMaterial->GetMaterialPass() == emberCommon::MaterialPass::deferredGeometry);
	}
	DeferredDrawCall::~DeferredDrawCall()
	{

	}
}