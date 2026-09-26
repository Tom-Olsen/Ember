#pragma once
#include "vulkanDescriptorSetBindingHandle.h"
#include <cstdint>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Mesh;



	struct OutlineDrawCall
	{
	public: // Members:
		Mesh* pMesh;
		DescriptorSetBindingHandle descriptorSetBindingHandle;
		uint32_t instanceCount;

	public: // Methods:
		OutlineDrawCall(Mesh* pMesh, const DescriptorSetBindingHandle& descriptorSetBindingHandle, uint32_t instanceCount);
		~OutlineDrawCall();
	};
}