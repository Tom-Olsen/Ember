#pragma once



// Global Initialization structure:
// Application::Application()
//     SystemInitializer::Init()
//         emberLogger::Logger::Init()
//         math::Random::Init()
//         emberTaskSyste::TaskSystem::Init()
//         Window::Init()
//         EventSystem::Init()
//         ComputeShaderManager::Init()
//         MaterialManager::Init()
//         BufferManager::Init()
//         TextureManager::Init()
//         SampleManager::Init()
//         MeshManager::Init()
// 
//     Renderer::Renderer()
//         Context::Init()
//             Lighting::Init()
//             Compute::Init()
//                 ImmediateCompute::Init()
//                 PostRenderCompute::Init()
//                 PreRenderCompute::Init()
//             RenderPassManager::Init()
//             GarbageCollector::Init()
//             Instance::Init()
//             PhysicalDevice::Init()
//             Surface::Init()
//             LogicalDevice::Init()
//             Allocator::Init()
//             AllocationTracker::Init()
//             DescriptorPool::Init()
//             SingleTimeCommand::Init()
//             DefaultGpuResources::Init()
//             PoolManager::Init()
//             Graphics::Init()



namespace emberEngine
{
    class SystemInitializer
    {
    public: // Methods
        static void Init(int windowWidth, int windowHeight);
        static void Clear();

    private: // Methods
        // Delete all constructors:
        SystemInitializer() = delete;
        SystemInitializer(const SystemInitializer&) = delete;
        SystemInitializer& operator=(const SystemInitializer&) = delete;
        SystemInitializer(SystemInitializer&&) = delete;
        SystemInitializer& operator=(SystemInitializer&&) = delete;
        ~SystemInitializer() = delete;
    };
}