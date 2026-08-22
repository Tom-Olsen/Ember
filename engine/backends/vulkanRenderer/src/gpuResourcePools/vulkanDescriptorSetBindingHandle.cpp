#include "vulkanDescriptorSetBindingHandle.h"



namespace vulkanRendererBackend
{
	// Constructors/Destructor:
	DescriptorSetBindingHandle::DescriptorSetBindingHandle()
		: m_pDescriptorSetBinding(nullptr)
	{

	}
	DescriptorSetBindingHandle::DescriptorSetBindingHandle(DescriptorSetBinding* pDescriptorSetBinding)
		: m_pDescriptorSetBinding(pDescriptorSetBinding)
	{

	}
	DescriptorSetBindingHandle::DescriptorSetBindingHandle(const ShaderHandle& poolShaderHandle, DescriptorSetBinding* pDescriptorSetBinding)
		: m_pDescriptorSetBinding(pDescriptorSetBinding), m_poolShaderHandle(poolShaderHandle)
	{

	}
	DescriptorSetBindingHandle::~DescriptorSetBindingHandle()
	{

	}



	// Getters:
	DescriptorSetBinding* DescriptorSetBindingHandle::Get() const
	{
		return m_pDescriptorSetBinding;
	}
	const ShaderHandle& DescriptorSetBindingHandle::GetPoolShaderHandle() const
	{
		return m_poolShaderHandle;
	}
	bool DescriptorSetBindingHandle::IsValid() const
	{
		return m_pDescriptorSetBinding != nullptr;
	}
	bool DescriptorSetBindingHandle::IsPooled() const
	{
		return !m_poolShaderHandle.IsEmpty() && IsValid();
	}
}