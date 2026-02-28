#pragma once
#include <memory>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class DepthTexture2dArray;
    class Material;
    class Mesh;
    class Sampler;
    class SampleTextureCube;
    class SampleTexture2d;
    class DescriptorSetBinding;
    class StorageBuffer;
    class StorageTexture2d;



    class DefaultGpuResources
    {
    private: // Members:
        static bool s_isInitialized;
        // Samplers:
        static std::unique_ptr<Sampler> s_pColorSampler;
        static std::unique_ptr<Sampler> s_pShadowSampler;
        // Materials:
        static std::unique_ptr<Material> s_pDefaultMaterial;
        static std::unique_ptr<Material> s_pDefaultPresentMaterial;
        // DescriptorSetBindings:
        static std::unique_ptr<DescriptorSetBinding> s_pGlobalDescriptorSetBinding;
        static std::unique_ptr<DescriptorSetBinding> s_pFrameDescriptorSetBinding;
        // Meshes:
        static std::unique_ptr<Mesh> s_pDefaultRenderQuad;
        // Buffers:
        static std::unique_ptr<StorageBuffer> s_pDefaultStorageBuffer;
        // Textures:
        static std::unique_ptr<SampleTexture2d> s_pDefaultSampleTexture2d;
        static std::unique_ptr<SampleTexture2d> s_pNormalMapSampleTexture2d;
        static std::unique_ptr<SampleTextureCube> s_pDefaultSampleTextureCube;
        static std::unique_ptr<DepthTexture2dArray> s_pDefaultDepthTexture2dArray;
        static std::unique_ptr<StorageTexture2d> s_pDefaultStorageTexture2d;

    public: // Methods:
        static void Init();
        static void Clear();

        // Samplers:
        static Sampler* GetColorSampler();
        static Sampler* GetShadowSampler();
        // Materials:
        static Material* GetDefaultMaterial();
        static Material* GetDefaultPresentMaterial();
        // DescriptorSetBindings:
        static DescriptorSetBinding* GetGlobalDescriptorSetBinding();
        static DescriptorSetBinding* GetFrameDescriptorSetBinding();
        // Meshes:
        static Mesh* GetDefaultRenderQuad();
        // Buffers:
        static StorageBuffer* GetDefaultStorageBuffer();
        static SampleTexture2d* GetDefaultSampleTexture2d();
        static SampleTexture2d* GetNormalMapSampleTexture2d();
        static SampleTextureCube* GetDefaultSampleTextureCube();
        static DepthTexture2dArray* GetDefaultDepthTexture2dArray();
        static StorageTexture2d* GetDefaultStorageTexture2d();

    private: // Methods
        void CreateDefaultRenderQuad();
        // Delete all constructors:
        DefaultGpuResources() = delete;
        DefaultGpuResources(const DefaultGpuResources&) = delete;
        DefaultGpuResources& operator=(const DefaultGpuResources&) = delete;
        DefaultGpuResources(DefaultGpuResources&&) = delete;
        DefaultGpuResources& operator=(DefaultGpuResources&&) = delete;
        ~DefaultGpuResources() = delete;
    };
}