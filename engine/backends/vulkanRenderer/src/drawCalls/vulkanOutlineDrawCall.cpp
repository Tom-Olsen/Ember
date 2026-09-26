#include "vulkanOutlineDrawCall.h"
#include <cassert>



namespace vulkanRendererBackend
{
	OutlineDrawCall::OutlineDrawCall(Mesh* pMesh, const DescriptorSetBindingHandle& descriptorSetBindingHandle, uint32_t instanceCount)
		: pMesh(pMesh)
		, descriptorSetBindingHandle(descriptorSetBindingHandle)
		, instanceCount(instanceCount)
	{
		assert(pMesh != nullptr);
		assert(descriptorSetBindingHandle.IsValid());
	}
	OutlineDrawCall::~OutlineDrawCall()
	{

	}
}