#include "vulkanOutlineDrawCall.h"
#include "vulkanDescriptorSetBinding.h"
#include <cassert>



namespace vulkanRendererBackend
{
	OutlineDrawCall::OutlineDrawCall(const Float4x4& localToWorldMatrix, Mesh* pMesh, const DescriptorSetBindingHandle& descriptorSetBindingHandle, uint32_t instanceCount)
		: localToWorldMatrix(localToWorldMatrix)
		, pMesh(pMesh)
		, descriptorSetBindingHandle(descriptorSetBindingHandle)
		, instanceCount(instanceCount)
	{
		assert(pMesh != nullptr);
		assert(descriptorSetBindingHandle.IsValid());
	}
	OutlineDrawCall::~OutlineDrawCall()
	{

	}



	void OutlineDrawCall::UpdateModelData()
	{
		DescriptorSetBinding* pDescriptorSetBinding = descriptorSetBindingHandle.Get();
		assert(pDescriptorSetBinding != nullptr);
		if (!pDescriptorSetBinding->HasBinding("ModelMatrizes"))
			return;

		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_localToWorldMatrix", localToWorldMatrix);
		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_worldToLocalMatrix", localToWorldMatrix.Inverse());
	}
}