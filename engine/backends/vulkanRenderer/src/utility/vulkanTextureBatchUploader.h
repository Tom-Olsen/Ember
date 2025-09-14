#pragma once
#include <filesystem>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class StagingBuffer;
    class Texture2d;



    class TextureBatchUploader
    {
    private: // Structs:
        struct PendingTexture
        {
            std::unique_ptr<StagingBuffer> pStagingBuffer;
            Texture2d* pTexture2d;  // replace with polymorphic Texture class.
        };

    private: // Members:
        std::vector<PendingTexture> m_pendingTextures;

    public: // Methods:
        TextureBatchUploader();
        ~TextureBatchUploader();

        // Non-copyable:
        TextureBatchUploader(const TextureBatchUploader&) = delete;
        TextureBatchUploader& operator=(const TextureBatchUploader&) = delete;

        // DepthTexture2dArray:
        TextureBatchUploader(TextureBatchUploader&& other) noexcept = default;
        TextureBatchUploader& operator=(TextureBatchUploader&& other) noexcept = default;

        void EnqueueTexture(StagingBuffer* pStagingBuffer, Texture2d* pTexture2d);
        void UploadTextures();
    };
}