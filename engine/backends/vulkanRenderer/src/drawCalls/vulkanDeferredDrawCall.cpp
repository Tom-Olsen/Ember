#include "vulkanDeferredDrawCall.h"
#include "commonCullMode.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanMaterial.h"
#include <cassert>



namespace vulkanRendererBackend
{
	DeferredDrawCall::DeferredDrawCall(const Float4x4& localToWorldMatrix, Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount)
		: localToWorldMatrix(localToWorldMatrix)
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
		assert(pMaterial->GetMaterialPass() == emberCommon::MaterialPass::deferredGeometry);
	}
	DeferredDrawCall::~DeferredDrawCall()
	{

	}



	void DeferredDrawCall::UpdateModelData()
	{
		DescriptorSetBinding* pDescriptorSetBinding = descriptorSetBindingHandle.Get();
		assert(pDescriptorSetBinding != nullptr);
		if (!pDescriptorSetBinding->HasBinding("ModelMatrizes"))
			return;

		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_localToWorldMatrix", localToWorldMatrix);
		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_worldToLocalMatrix", localToWorldMatrix.Inverse());
	}
}