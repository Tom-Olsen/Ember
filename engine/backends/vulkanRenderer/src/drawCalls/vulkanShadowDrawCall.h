#pragma once
#include "emberMath.h"
#include "vulkanDescriptorSetBindingHandle.h"
#include <cstdint>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class Mesh;



	struct ShadowDrawCall
	{
	public: // Members:
		Float4x4 localToWorldMatrix;
		Mesh* pMesh;
		Material* pMaterial;
		DescriptorSetBindingHandle descriptorSetBindingHandle;
		uint32_t instanceCount;

	public: // Methods:
		ShadowDrawCall(const Float4x4& localToWorldMatrix, Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, uint32_t instanceCount);
		~ShadowDrawCall();

		void UpdateModelData();
	};
}