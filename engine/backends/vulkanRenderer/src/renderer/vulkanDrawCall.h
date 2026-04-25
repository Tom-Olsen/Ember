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
		DescriptorSetBinding* pCallDescriptorSetBinding;
		DescriptorSetBinding* pShadowDescriptorSetBinding;
		Mesh* pMesh;
		uint32_t instanceCount;	// 0 implies no instanced rendering.
	};
}