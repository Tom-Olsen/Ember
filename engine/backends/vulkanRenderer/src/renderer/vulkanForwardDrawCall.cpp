#include "vulkanForwardDrawCall.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanMaterial.h"
#include <cassert>



namespace vulkanRendererBackend
{
	ForwardDrawCall::ForwardDrawCall(const Float4x4& localToWorldMatrix, Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, bool receiveShadows, uint32_t instanceCount)
		: localToWorldMatrix(localToWorldMatrix)
		, pMesh(pMesh)
		, pMaterial(pMaterial)
		, descriptorSetBindingHandle(descriptorSetBindingHandle)
		, receiveShadows(receiveShadows)
		, instanceCount(instanceCount)
	{
		assert(pMesh != nullptr);
		assert(pMaterial != nullptr);
		assert(descriptorSetBindingHandle.IsValid());
		assert(pMaterial->GetMaterialPass() == emberCommon::MaterialPass::forward);
	}
	ForwardDrawCall::~ForwardDrawCall()
	{

	}



	void ForwardDrawCall::UpdateModelData()
	{
		DescriptorSetBinding* pDescriptorSetBinding = descriptorSetBindingHandle.Get();
    	assert(pDescriptorSetBinding != nullptr);
    	if (!pDescriptorSetBinding->HasBinding("ModelMatrizes"))
    	    return;

		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_localToWorldMatrix", localToWorldMatrix);
		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_worldToLocalMatrix", localToWorldMatrix.Inverse());
	}
}