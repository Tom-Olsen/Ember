#include "vulkanTextureBatchUploader.h"
#include "stb_image.h"
#include "texture2d.h"



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		TextureBatchUploader::TextureBatchUploader()
		{

		}
		TextureBatchUploader::~TextureBatchUploader()
		{

		}



		// Public methods:
		void TextureBatchUploader::EnqueueTexture(VkFormat format, const std::filesystem::path& filePath)
		{
			std::scoped_lock lock(m_mutex);
			m_pendingTextures.push_back({ format, filePath });
		}
		void TextureBatchUploader::UploadTextures()
		{

		}


		// Private methods:
		void TextureBatchUploader::UploadTexture(const PendingTexture& pendingTexture)
		{
			//// Load image:
			//stbi_set_flip_vertically_on_load(true);
			//stbi_uc* pPixels = stbi_load(pendingTexture.filePath.string().c_str(), &m_width, &m_height, nullptr, m_channels);
			//if (!pPixels)
			//	throw std::runtime_error("Failed to load texture image!");
		}
	}
}