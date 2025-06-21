#ifndef __INCLUDE_GUARD_vulkanTextureBatchUploader_h__
#define __INCLUDE_GUARD_vulkanTextureBatchUploader_h__
#include <filesystem>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
    // Forward declerations:
    class StagingBuffer;
    class Texture2d;



    namespace vulkanBackend
    {
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
            
            void EnqueueTexture(StagingBuffer* pStagingBuffer, Texture2d* pTexture2d);
            void UploadTextures();
        };
    }
}



#endif // __INCLUDE_GUARD_vulkanTextureBatchUploader_h__