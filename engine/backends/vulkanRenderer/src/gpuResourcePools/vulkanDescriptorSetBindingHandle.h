#pragma once



namespace vulkanRendererBackend
{
    // Forward declarations:
    class DescriptorSetBinding;
    class Shader;



    class DescriptorSetBindingHandle
    {
    private: // Members:
        DescriptorSetBinding* m_pDescriptorSetBinding;
        Shader* m_pPoolShader;  // nullptr if m_pDescriptorSetBinding does not originate from a pool.

    public: // Methods:
        // Constructors/Destructor:
        DescriptorSetBindingHandle();
        explicit DescriptorSetBindingHandle(DescriptorSetBinding* pDescriptorSetBinding);
        DescriptorSetBindingHandle(Shader* pPoolShader, DescriptorSetBinding* pDescriptorSetBinding);
        ~DescriptorSetBindingHandle();

        // Getters:
        DescriptorSetBinding* Get() const;
        Shader* GetPoolShader() const;
        bool IsValid() const;
        bool IsPooled() const;
    };
}