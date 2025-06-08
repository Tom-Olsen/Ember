#ifndef __INCLUDE_GUARD_vulkanTextureBatchUploader_h__
#define __INCLUDE_GUARD_vulkanTextureBatchUploader_h__
#include <filesystem>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
    namespace vulkanBackend
    {
        class TextureBatchUploader
        {
        private: // Structs:
            struct PendingTexture
            {
                VkFormat format;
                std::filesystem::path filePath;
            };

        private: // Members:
            std::mutex m_mutex;
            std::vector<PendingTexture> m_pendingTextures;

        public: // Methods:
            TextureBatchUploader();
            ~TextureBatchUploader();
            
            void EnqueueTexture(VkFormat format, const std::filesystem::path& filePath);
            void UploadTextures();
            
        private: // Methods:
            void UploadTexture(const PendingTexture& texture);
        };
    }
}



#endif // __INCLUDE_GUARD_vulkanTextureBatchUploader_h__