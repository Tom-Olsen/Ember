#pragma once
#include "emberMath.h"
#include "vulkanDescriptorSetBindingHandle.h"
#include <cstdint>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Mesh;



	struct OutlineDrawCall
	{
	public: // Members:
		Float4x4 localToWorldMatrix;
		Mesh* pMesh;
		DescriptorSetBindingHandle descriptorSetBindingHandle;
		uint32_t instanceCount;

	public: // Methods:
		OutlineDrawCall(const Float4x4& localToWorldMatrix, Mesh* pMesh, const DescriptorSetBindingHandle& descriptorSetBindingHandle, uint32_t instanceCount);
		~OutlineDrawCall();

		void UpdateModelData();
	};
}