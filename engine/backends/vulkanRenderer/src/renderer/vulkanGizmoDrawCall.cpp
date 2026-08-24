#include "vulkanGizmoDrawCall.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanMaterial.h"
#include <cassert>



namespace vulkanRendererBackend
{
	GizmoDrawCall::GizmoDrawCall(const Float4x4& localToWorldMatrix, Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, uint32_t instanceCount)
		: localToWorldMatrix(localToWorldMatrix)
		, pMesh(pMesh)
		, pMaterial(pMaterial)
		, descriptorSetBindingHandle(descriptorSetBindingHandle)
		, instanceCount(instanceCount)
	{
		assert(pMesh != nullptr);
		assert(pMaterial != nullptr);
		assert(descriptorSetBindingHandle.IsValid());
		assert(pMaterial->GetMaterialPass() == emberCommon::MaterialPass::gizmo);
	}
	GizmoDrawCall::~GizmoDrawCall()
	{

	}



	void GizmoDrawCall::UpdateModelData()
	{
		DescriptorSetBinding* pDescriptorSetBinding = descriptorSetBindingHandle.Get();
		assert(pDescriptorSetBinding != nullptr);
		if (!pDescriptorSetBinding->HasBinding("ModelMatrizes"))
			return;

		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_localToWorldMatrix", localToWorldMatrix);
		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_worldToLocalMatrix", localToWorldMatrix.Inverse());
	}
}