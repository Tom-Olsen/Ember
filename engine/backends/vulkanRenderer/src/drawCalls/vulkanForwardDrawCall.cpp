#include "vulkanForwardDrawCall.h"
#include "commonCullMode.h"
#include "vulkanMaterial.h"
#include <cassert>



namespace vulkanRendererBackend
{
	ForwardDrawCall::ForwardDrawCall(const Float3& worldPosition, Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount)
		: worldPosition(worldPosition)
		, pMesh(pMesh)
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
		assert(pMaterial->GetMaterialPass() == emberCommon::MaterialPass::forward);
	}
	ForwardDrawCall::~ForwardDrawCall()
	{

	}
}