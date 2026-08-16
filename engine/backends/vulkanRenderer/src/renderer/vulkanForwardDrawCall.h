#pragma once
#include "emberMath.h"
#include "vulkanDescriptorSetBindingHandle.h"
#include <cstdint>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class Mesh;



	struct ForwardDrawCall
	{
	public: // Members:
		Float4x4 localToWorldMatrix;
		Mesh* pMesh;
		Material* pMaterial;
		DescriptorSetBindingHandle descriptorSetBindingHandle;
		bool receiveShadows;
		uint32_t instanceCount;

	public: // Methods:
		ForwardDrawCall(const Float4x4& localToWorldMatrix, Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, bool receiveShadows, uint32_t instanceCount);
		~ForwardDrawCall();

		void UpdateModelData();
	};
}