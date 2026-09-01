#pragma once
#include "emberMath.h"
#include "vulkanDescriptorSetBindingHandle.h"
#include <cstdint>



// Forward declarations:
namespace emberCommon
{
	enum class CullMode;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class Mesh;



	struct DeferredDrawCall
	{
	public: // Members:
		Float4x4 localToWorldMatrix;
		Mesh* pMesh;
		Material* pMaterial;
		DescriptorSetBindingHandle descriptorSetBindingHandle;
		emberCommon::CullMode cullMode;
		bool receiveShadows;
		uint32_t instanceCount;

	public: // Methods:
		DeferredDrawCall(const Float4x4& localToWorldMatrix, Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount);
		~DeferredDrawCall();

		void UpdateModelData();
	};
}