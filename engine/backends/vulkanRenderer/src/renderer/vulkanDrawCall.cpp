#include "vulkanDrawCall.h"
#include "vulkanDescriptorSetBinding.h"



namespace vulkanRendererBackend
{
	void DrawCall::SetModelData()
	{
		bool callHasModelDataBinding = HasModelDataBinding(pCallDescriptorSetBinding);
		bool shadowHasModelDataBinding = HasModelDataBinding(pShadowDescriptorSetBinding);
		if (!callHasModelDataBinding && !shadowHasModelDataBinding)
			return;

		Float4x4 worldToLocalMatrix = localToWorldMatrix.Inverse();
		if (callHasModelDataBinding)
			SetModelData(pCallDescriptorSetBinding, worldToLocalMatrix);
		if (shadowHasModelDataBinding)
			SetModelData(pShadowDescriptorSetBinding, worldToLocalMatrix);
	}
	bool DrawCall::HasModelDataBinding(DescriptorSetBinding* pDescriptorSetBinding)
	{
		return pDescriptorSetBinding && pDescriptorSetBinding->HasBinding("ModelMatrizes");
	}
	void DrawCall::SetModelData(DescriptorSetBinding* pDescriptorSetBinding, const Float4x4& worldToLocalMatrix)
	{
		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_localToWorldMatrix", localToWorldMatrix);
		pDescriptorSetBinding->SetFloat4x4("ModelMatrizes", "model_worldToLocalMatrix", worldToLocalMatrix);
	}
}