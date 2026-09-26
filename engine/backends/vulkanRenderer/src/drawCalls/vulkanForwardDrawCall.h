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



	struct ForwardDrawCall
	{
	public: // Members:
		Float3 worldPosition;
		Mesh* pMesh;
		Material* pMaterial;
		DescriptorSetBindingHandle descriptorSetBindingHandle;
		emberCommon::CullMode cullMode;
		bool receiveShadows;
		uint32_t instanceCount;

	public: // Methods:
		ForwardDrawCall(const Float3& worldPosition, Mesh* pMesh, Material* pMaterial, const DescriptorSetBindingHandle& descriptorSetBindingHandle, emberCommon::CullMode cullMode, bool receiveShadows, uint32_t instanceCount);
		~ForwardDrawCall();
	};
}