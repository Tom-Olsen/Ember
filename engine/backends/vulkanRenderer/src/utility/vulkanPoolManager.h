#pragma once
#include "vulkanCallDescriptorSetBindingPool.h"
#include "vulkanStagingBufferPool.h"
#include <cstdint>
#include <map>
#include <unordered_map>



namespace vulkanRendererBackend
{
    // Forward declarations:
    class DescriptorSetBinding;
    class ComputeShader;
    class Shader;
    class StagingBuffer;



    /// <summary>
    /// Purely static class that takes care of custom resource pools.
    /// CallDescriptorSetBindings and StagingBuffers are special, as they get one pool per:
    /// -CallDescriptorSetBindings: Shader (material or computeShader)
    /// -StagingBuffer: size (rounded up to next power of two)
    /// </summary>
    class PoolManager
    {
    private: // Members
        static bool s_isInitialized;
		static std::unordered_map<Shader*, CallDescriptorSetBindingPool> s_callDescriptorSetBindingPoolMap;
        static std::map<uint32_t, StagingBufferPool> s_stagingBufferPoolMap;

    public: // Methods
	    // Initialization/Cleanup:
        static void Init();
        static void Clear();

        // Checkout:
        static DescriptorSetBinding* CheckOutCallDescriptorSetBinding(Shader* pShader);
        static StagingBuffer* CheckOutStagingBuffer(uint32_t size);

        // Return:
        static void ReturnCallDescriptorSetBinding(Shader* pShader, DescriptorSetBinding* pDescriptorSetBinding);
        static void ReturnStagingBuffer(uint32_t size, StagingBuffer* pStagingBuffer);

        // Debugging:
        static void PrintCallDescriptorSetBindingPoolState();
        static void PrintStagingBufferPoolState();

    private: // Methods
        static bool IsValidCallDescriptorSetBindingShader(const Shader* pShader);
        static uint16_t GetAvailableStorageBufferCount(uint32_t size);

        // Delete all constructors:
        PoolManager() = delete;
        PoolManager(const PoolManager&) = delete;
        PoolManager& operator=(const PoolManager&) = delete;
        PoolManager(PoolManager&&) = delete;
        PoolManager& operator=(PoolManager&&) = delete;
        ~PoolManager() = delete;
    };
}