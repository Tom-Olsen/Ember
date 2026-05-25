#pragma once
#include "commonLighting.h"
#include "emberMath.h"
#include <vector>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class DescriptorSetBinding;
	class Mesh;



	struct DrawCall
	{
		Float4x4 localToWorldMatrix;
		bool receiveShadows;
		bool castShadows;
		Material* pMaterial;
		bool ownsDescriptorSetBinding;					    // True if pCallDescriptorSetBinding was borrowed from pool.
		bool ownsShadowDescriptorSetBinding;				// True if pShadowDescriptorSetBinding was borrowed from pool.
		DescriptorSetBinding* pCallDescriptorSetBinding;
		DescriptorSetBinding* pShadowDescriptorSetBinding;
		Mesh* pMesh;
		uint32_t instanceCount;	// 0 implies no instanced rendering.

		void SetModelData();

	private: // Methods:
		bool HasModelDataBinding(DescriptorSetBinding* pDescriptorSetBinding);
		void SetModelData(DescriptorSetBinding* pDescriptorSetBinding, const Float4x4& worldToLocalMatrix);
	};
}