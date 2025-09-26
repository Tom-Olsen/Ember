#pragma once
#include <memory>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class DepthTexture2dArray;
    class Sampler;
    class SampleTextureCube;
    class SampleTexture2d;
    class StorageBuffer;
    class StorageTexture2d;



    class DefaultGpuResources
    {
    private: // Members:
        static std::unique_ptr<Sampler> s_pColorSampler;
        static std::unique_ptr<Sampler> s_pShadowSampler;
        static std::unique_ptr<StorageBuffer> s_pDefaultStorageBuffer;
        static std::unique_ptr<SampleTexture2d> s_pDefaultSampleTexture2d;
        static std::unique_ptr<SampleTexture2d> s_pNormalMapSampleTexture2d;
        static std::unique_ptr<SampleTextureCube> s_pDefaultSampleTextureCube;
        static std::unique_ptr<DepthTexture2dArray> s_pDefaultDepthTexture2dArray;
        static std::unique_ptr<StorageTexture2d> s_pDefaultStorageTexture2d;


    public: // Methods:
        static void Init();
        static void Clear();

        static Sampler* GetColorSampler();
        static Sampler* GetShadowSampler();
        static StorageBuffer* GetDefaultStorageBuffer();
        static SampleTexture2d* GetDefaultSampleTexture2d();
        static SampleTexture2d* GetNormalMapSampleTexture2d();
        static SampleTextureCube* GetDefaultSampleTextureCube();
        static DepthTexture2dArray* GetDefaultDepthTexture2dArray();
        static StorageTexture2d* GetDefaultStorageTexture2d();


    private: // Methods
        // Delete all constructors:
        DefaultGpuResources() = delete;
        DefaultGpuResources(const DefaultGpuResources&) = delete;
        DefaultGpuResources& operator=(const DefaultGpuResources&) = delete;
        DefaultGpuResources(DefaultGpuResources&&) = delete;
        DefaultGpuResources& operator=(DefaultGpuResources&&) = delete;
        ~DefaultGpuResources() = delete;
    };
}