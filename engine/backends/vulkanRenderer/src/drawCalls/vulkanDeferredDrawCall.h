#pragma once
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
		Mesh* pMesh;
		Material* pMaterial;
		DescriptorSetBindingHandle descriptorSetBindingHandle;
		emberCommon::CullMode cullMode;
		bool receiveShadows;
		uint32_t instanceCount;

	public: // Methods:
		DeferredDrawCall(Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount);
		~DeferredDrawCall();
	};
}