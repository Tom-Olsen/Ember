#include "vulkanTextureBatchUploader.h"
#include "stb_image.h"
#include "sampleTexture2d.h"
#include "stagingBuffer.h"
#include "vulkanContext.h"
#include "vulkanPipelineStages.h"
#include "vulkanSingleTimeCommand.h"



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
		void TextureBatchUploader::EnqueueTexture(StagingBuffer* pStagingBuffer, SampleTexture2d* pSampleTexture2d)
		{
			m_pendingTextures.push_back({ std::unique_ptr<StagingBuffer>(pStagingBuffer), pSampleTexture2d });
		}
		void TextureBatchUploader::UploadTextures()
		{

		}


		// Private methods:
		void TextureBatchUploader::UploadTexture(const PendingTexture& pendingTexture)
		{
			VkCommandBuffer transferCommandBuffer = SingleTimeCommand::BeginCommand(Context::logicalDevice.GetTransferQueue());
			VkCommandBuffer graphicsCommandBuffer = SingleTimeCommand::BeginCommand(Context::logicalDevice.GetGraphicsQueue());
			for (auto& pendingTexture: m_pendingTextures)
				pendingTexture.pSampleTexture2d->RecordGpuCommands(transferCommandBuffer, graphicsCommandBuffer, pendingTexture.pStagingBuffer.get());
			SingleTimeCommand::EndLinkedCommands(Context::logicalDevice.GetTransferQueue(), Context::logicalDevice.GetGraphicsQueue(), pipelineStage::transfer);
		}
	}
}