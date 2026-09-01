#pragma once
#include <cstdint>
#include <memory>



namespace emberBackendInterface
{
    class IMaterial;
}



namespace vulkanRendererBackend
{
    // Forward declarations:
    class ComputeShader;
    class DepthTexture2dArray;
    class Material;
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
        static std::unique_ptr<Sampler> s_pColorSamplerClampEdge;
        static std::unique_ptr<Sampler> s_pShadowSampler;
        // Materials:
        static Material* s_pDefaultOutlineMaterial;
        static Material* s_pDefaultShadowMaterial;
        static Material* s_pDefaultDeferredLightingMaterial;
        static Material* s_pDefaultPresentMaterial;
        // Compute shaders:
        static std::unique_ptr<ComputeShader> s_pGammaCorrectionComputeShader;
        static std::unique_ptr<ComputeShader> s_pOutlineComputeShader;
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
        static void Init();
        static void Clear();

		// Set/Clear default materials:
        static void SetDefaultMaterials(
			emberBackendInterface::IMaterial* pOutlineMaterial,
			emberBackendInterface::IMaterial* pDefaultShadowMaterial,
			emberBackendInterface::IMaterial* pDeferredLightingMaterial, 
			emberBackendInterface::IMaterial* pPresentMaterial);
		static void ClearDefaultMaterials();

        // Samplers:
        static Sampler* GetColorSampler();
        static Sampler* GetColorSamplerClampEdge();
        static Sampler* GetShadowSampler();
        // Materials:
        static Material* GetDefaultOutlineMaterial();
        static Material* GetDefaultShadowMaterial();
        static Material* GetDefaultDeferredLightingMaterial();
        static Material* GetDefaultPresentMaterial();
        // Compute shaders:
        static ComputeShader* GetGammaCorrectionComputeShader();
        static ComputeShader* GetOutlineComputeShader();
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
        // Delete all constructors:
        DefaultGpuResources() = delete;
        DefaultGpuResources(const DefaultGpuResources&) = delete;
        DefaultGpuResources& operator=(const DefaultGpuResources&) = delete;
        DefaultGpuResources(DefaultGpuResources&&) = delete;
        DefaultGpuResources& operator=(DefaultGpuResources&&) = delete;
        ~DefaultGpuResources() = delete;
    };
}