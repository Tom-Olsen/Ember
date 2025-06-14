#ifndef __INCLUDE_GUARD_vulkanTextureBatchUploader_h__
#define __INCLUDE_GUARD_vulkanTextureBatchUploader_h__
#include <filesystem>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
    // Forward declerations:
    class StagingBuffer;
    class SampleTexture2d;



    namespace vulkanBackend
    {
        class TextureBatchUploader
        {
        private: // Structs:
            struct PendingTexture
            {
                std::unique_ptr<StagingBuffer> pStagingBuffer;
                SampleTexture2d* pSampleTexture2d;  // replace with polymorphic Texture class.
            };

        private: // Members:
            std::vector<PendingTexture> m_pendingTextures;

        public: // Methods:
            TextureBatchUploader();
            ~TextureBatchUploader();
            
            void EnqueueTexture(StagingBuffer* pStagingBuffer, SampleTexture2d* pSampleTexture2d);
            void UploadTextures();
            
        private: // Methods:
            void UploadTexture(const PendingTexture& texture);
        };
    }
}



#endif // __INCLUDE_GUARD_vulkanTextureBatchUploader_h__