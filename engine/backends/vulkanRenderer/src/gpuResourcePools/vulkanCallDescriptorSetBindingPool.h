#pragma once
#include <memory>
#include <queue>
#include <vector>



namespace vulkanRendererBackend
{
    // Forward declarations:
    class Shader;
    class DescriptorSetBinding;



    /// <summary>
    /// Use one call descriptor set binding pool per Shader*.
    /// </summary>
    class CallDescriptorSetBindingPool
    {
    private: // Members:
        std::vector<DescriptorSetBinding*> m_storage;   // Stores DescriptorSetBinding*
        std::queue<DescriptorSetBinding*> m_pool;       // Queue of available DescriptorSetBinding*.
        size_t m_currentUsage;  // Tracks how many call descriptor set bindings have been handed out at any moment.
        size_t m_peakUsage;     // Tracks peak usage.

    public: // Methods:
        CallDescriptorSetBindingPool();
        ~CallDescriptorSetBindingPool();

        // Non-copyable:
        CallDescriptorSetBindingPool(const CallDescriptorSetBindingPool&) = delete;
        CallDescriptorSetBindingPool& operator=(const CallDescriptorSetBindingPool&) = delete;

        // Movable:
        CallDescriptorSetBindingPool(CallDescriptorSetBindingPool&& other) noexcept = default;
        CallDescriptorSetBindingPool& operator=(CallDescriptorSetBindingPool&& other) noexcept = default;

        // Checkout/Return:
        DescriptorSetBinding* CheckOut(Shader* pShader);
        void Return(DescriptorSetBinding* pDescriptorSetBinding);

        // Debugging:
        void PrintPoolState();
    };
}