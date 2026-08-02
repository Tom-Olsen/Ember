#pragma once
#include "commonMaterialRenderState.h"
#include "emberMath.h"
#include "vulkanDescriptorSetBindingHandle.h"
#include <cstdint>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class DescriptorSetBinding;
	class Mesh;



	class DrawCall
	{
    public: // Structs:
        struct MaterialState
        {
            Material* pMaterial;
            DescriptorSetBindingHandle descriptorSetBindingHandle;
            emberCommon::MaterialRenderState renderState;

            MaterialState();
            MaterialState(Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle);
        };
        struct ShadowState
        {
            Material* pMaterial;
            DescriptorSetBindingHandle descriptorSetBindingHandle;
            bool receiveShadows;
            bool castShadows;

            ShadowState();
            ShadowState(Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, bool receiveShadows, bool castShadows);
        };

    public: // Members:
		Float4x4 localToWorldMatrix;
        MaterialState materialState;
        ShadowState shadowState;
		Mesh* pMesh;
		uint32_t instanceCount;	// 0 means no instanced rendering.

    public: // Methods:
        // Constructors/Destructor:
        DrawCall(const Float4x4& localToWorldMatrix, const MaterialState& materialState, const ShadowState& shadowState, Mesh* pMesh, uint32_t instanceCount = 0);
        DrawCall(const Float4x4& localToWorldMatrix, const MaterialState& materialState, Mesh* pMesh, uint32_t instanceCount = 0);
        ~DrawCall();

		void SetModelData();

	private: // Methods:
		bool HasModelDataBinding(DescriptorSetBinding* pDescriptorSetBinding);
		void SetModelData(DescriptorSetBinding* pDescriptorSetBinding, const Float4x4& worldToLocalMatrix);
	};
}