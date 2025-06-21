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
			const DeviceQueue& transferQueue = Context::logicalDevice.GetTransferQueue();
			const DeviceQueue& graphicsQueue = Context::logicalDevice.GetGraphicsQueue();
			if (transferQueue.queue != graphicsQueue.queue)
			{
				VkCommandBuffer transferCommandBuffer = SingleTimeCommand::BeginCommand(transferQueue);
				VkCommandBuffer graphicsCommandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
				for (auto& pendingTexture : m_pendingTextures)
					pendingTexture.pSampleTexture2d->RecordGpuCommands(transferCommandBuffer, graphicsCommandBuffer, pendingTexture.pStagingBuffer.get());
				SingleTimeCommand::EndLinkedCommands(transferQueue, graphicsQueue, pipelineStage::transfer);
			}
			else
			{
				VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
				for (auto& pendingTexture : m_pendingTextures)
					pendingTexture.pSampleTexture2d->RecordGpuCommands(commandBuffer, commandBuffer, pendingTexture.pStagingBuffer.get());
				SingleTimeCommand::EndCommand(graphicsQueue);
			}
		}
	}
}