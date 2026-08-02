#include "vulkanDrawCall.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanMaterial.h"
#include <cassert>



namespace vulkanRendererBackend
{
    // Public structs:
	DrawCall::MaterialState::MaterialState() :
        pMaterial(nullptr),
        descriptorSetBindingHandle(),
        renderState()
	{

	}
	DrawCall::MaterialState::MaterialState(Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle) :
        pMaterial(pMaterial),
        descriptorSetBindingHandle(descriptorSetBindingHandle),
        renderState(pMaterial ? pMaterial->GetDefaultRenderState() : emberCommon::MaterialRenderState())
	{
        assert(pMaterial != nullptr);
	}
	DrawCall::ShadowState::ShadowState() :
        pMaterial(nullptr),
        descriptorSetBindingHandle(),
        receiveShadows(false),
        castShadows(false)
	{

	}
	DrawCall::ShadowState::ShadowState(Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, bool receiveShadows, bool castShadows) :
        pMaterial(pMaterial),
        descriptorSetBindingHandle(descriptorSetBindingHandle),
        receiveShadows(receiveShadows),
        castShadows(castShadows)
	{
        assert(pMaterial != nullptr);
	}



	// Constructor/Destructor:
	DrawCall::DrawCall(const Float4x4& localToWorldMatrix, const MaterialState& materialState, const ShadowState& shadowState, Mesh* pMesh, uint32_t instanceCount)
        : localToWorldMatrix(localToWorldMatrix), materialState(materialState), shadowState(shadowState), pMesh(pMesh), instanceCount(instanceCount)
	{
        assert(pMesh != nullptr);
	}
	DrawCall::DrawCall(const Float4x4& localToWorldMatrix, const MaterialState& materialState, Mesh* pMesh, uint32_t instanceCount)
        : localToWorldMatrix(localToWorldMatrix), materialState(materialState), shadowState(), pMesh(pMesh), instanceCount(instanceCount)
	{
        assert(pMesh != nullptr);
	}
	DrawCall::~DrawCall()
	{

	}



	void DrawCall::SetModelData()
	{
		DescriptorSetBinding* pMaterialDescriptorSetBinding = materialState.descriptorSetBindingHandle.Get();
		DescriptorSetBinding* pShadowDescriptorSetBinding = shadowState.descriptorSetBindingHandle.Get();
		bool callHasModelDataBinding = HasModelDataBinding(pMaterialDescriptorSetBinding);
		bool shadowHasModelDataBinding = HasModelDataBinding(pShadowDescriptorSetBinding);
		if (!callHasModelDataBinding && !shadowHasModelDataBinding)
			return;

		Float4x4 worldToLocalMatrix = localToWorldMatrix.Inverse();
		if (callHasModelDataBinding)
			SetModelData(pMaterialDescriptorSetBinding, worldToLocalMatrix);
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