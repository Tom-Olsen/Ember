#pragma once
#include <cstdint>
#include <memory>



namespace vulkanRendererBackend
{
    // Forward declarations:
    class ComputeShader;
    class DepthTexture2dArray;
    class Material;
    class Mesh;
    class Sampler;
    class SampleTextureCube;
    class SampleTexture2d;
    class SampleTexture3d;
    class DescriptorSetBinding;
    class StorageBuffer;
    class StorageTexture2d;
    class StorageTexture3d;



    class DefaultGpuResources
    {
    private: // Members:
        static bool s_isInitialized;
        // Samplers:
        static std::unique_ptr<Sampler> s_pColorSampler;
        static std::unique_ptr<Sampler> s_pShadowSampler;
        // Materials:
        static std::unique_ptr<Material> s_pDefaultPresentMaterial;
        static std::unique_ptr<Material> s_pDefaultShadowMaterial;
        // Compute shaders:
        static std::unique_ptr<ComputeShader> s_pGammaCorrectionComputeShader;
        // Meshes:
        static std::unique_ptr<Mesh> s_pDefaultRenderQuad;
        // Buffers:
        static std::unique_ptr<StorageBuffer> s_pDefaultStorageBuffer;
        // Textures:
        static std::unique_ptr<SampleTexture2d> s_pDefaultSampleTexture2d;
        static std::unique_ptr<SampleTexture2d> s_pDefaultNormalMap;
        static std::unique_ptr<SampleTexture3d> s_pDefaultSampleTexture3d;
        static std::unique_ptr<SampleTextureCube> s_pDefaultSampleTextureCube;
        static std::unique_ptr<DepthTexture2dArray> s_pDefaultDepthTexture2dArray;
        static std::unique_ptr<StorageTexture2d> s_pDefaultStorageTexture2d;
        static std::unique_ptr<StorageTexture3d> s_pDefaultStorageTexture3d;

    public: // Methods:
        static void InitSamplers();
        static void Init(uint32_t shadowMapResolution);
        static void Clear();

        // Samplers:
        static Sampler* GetColorSampler();
        static Sampler* GetShadowSampler();
        // Materials:
        static Material* GetDefaultPresentMaterial();
        static Material* GetDefaultShadowMaterial();
        // Compute shaders:
        static ComputeShader* GetGammaCorrectionComputeShader();
        // Meshes:
        static Mesh* GetDefaultRenderQuad();
        // Buffers:
        static StorageBuffer* GetDefaultStorageBuffer();
        static SampleTexture2d* GetDefaultSampleTexture2d();
        static SampleTexture2d* GetDefaultNormalMap();
        static SampleTexture3d* GetDefaultSampleTexture3d();
        static SampleTextureCube* GetDefaultSampleTextureCube();
        static DepthTexture2dArray* GetDefaultDepthTexture2dArray();
        static StorageTexture2d* GetDefaultStorageTexture2d();
        static StorageTexture3d* GetDefaultStorageTexture3d();

    private: // Methods
        static Mesh CreateDefaultRenderQuad();
        // Delete all constructors:
        DefaultGpuResources() = delete;
        DefaultGpuResources(const DefaultGpuResources&) = delete;
        DefaultGpuResources& operator=(const DefaultGpuResources&) = delete;
        DefaultGpuResources(DefaultGpuResources&&) = delete;
        DefaultGpuResources& operator=(DefaultGpuResources&&) = delete;
        ~DefaultGpuResources() = delete;
    };
}