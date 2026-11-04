#pragma once
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
		Mesh* pMesh;
		Material* pMaterial;
		DescriptorSetBindingHandle descriptorSetBindingHandle;
		uint32_t instanceCount;
		uint32_t submeshIndex;

	public: // Methods:
		ShadowDrawCall(Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, uint32_t instanceCount, uint32_t submeshIndex);
		~ShadowDrawCall();
	};
}