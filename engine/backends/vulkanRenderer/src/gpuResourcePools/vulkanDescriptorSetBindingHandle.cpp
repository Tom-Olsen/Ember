#include "vulkanDescriptorSetBindingHandle.h"



namespace vulkanRendererBackend
{
    // Constructors/Destructor:
    DescriptorSetBindingHandle::DescriptorSetBindingHandle()
        : m_pDescriptorSetBinding(nullptr), m_pPoolShader(nullptr)
    {

    }
    DescriptorSetBindingHandle::DescriptorSetBindingHandle(DescriptorSetBinding* pDescriptorSetBinding)
        : m_pDescriptorSetBinding(pDescriptorSetBinding), m_pPoolShader(nullptr)
    {

    }
    DescriptorSetBindingHandle::DescriptorSetBindingHandle(Shader* pPoolShader, DescriptorSetBinding* pDescriptorSetBinding)
        : m_pDescriptorSetBinding(pDescriptorSetBinding), m_pPoolShader(pPoolShader)
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
    Shader* DescriptorSetBindingHandle::GetPoolShader() const
    {
        return m_pPoolShader;
    }
    bool DescriptorSetBindingHandle::IsValid() const
    {
        return m_pDescriptorSetBinding != nullptr;
    }
    bool DescriptorSetBindingHandle::IsPooled() const
    {
        return m_pPoolShader != nullptr && IsValid();
    }
}