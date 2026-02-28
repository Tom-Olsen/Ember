#pragma once
#include <memory>
#include <queue>
#include <vector>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class Shader;
    class DescriptorSetBinding;



    /// <summary>
    /// Use one shader properties pool per Shader*.
    /// </summary>
    class DescriptorSetBindingPool
    {
    private: // Members:
        std::vector<DescriptorSetBinding*> m_storage;   // Stores DescriptorSetBinding*
        std::queue<DescriptorSetBinding*> m_pool;       // Queue of available DescriptorSetBinding*.
        size_t m_currentUsage;  // Tracks how many shader properties have been handed out at any moment.
        size_t m_peakUsage;     // Tracks peak usage.

    public: // Methods:
        DescriptorSetBindingPool();
        ~DescriptorSetBindingPool();

        // Non-copyable:
        DescriptorSetBindingPool(const DescriptorSetBindingPool&) = delete;
        DescriptorSetBindingPool& operator=(const DescriptorSetBindingPool&) = delete;

        // Movable:
        DescriptorSetBindingPool(DescriptorSetBindingPool&& other) noexcept = default;
        DescriptorSetBindingPool& operator=(DescriptorSetBindingPool&& other) noexcept = default;

        // Checkout/Return:
        DescriptorSetBinding* CheckOut(Shader* pShader);
        void Return(DescriptorSetBinding* pDescriptorSetBinding);

        // Debugging:
        void PrintPoolState();
    };
}