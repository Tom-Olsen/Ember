#include "vulkanTextureBatchUploader.h"
#include "stb_image.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStages.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStagingBuffer.h"
#include "vulkanTexture2d.h"



// Ember::ToDo: batch uploading currently disabled/removed. Reimplement with parallel taskflow streaming.


//namespace vulkanRendererBackend
//{
//	// Constructor/Destructor:
//	TextureBatchUploader::TextureBatchUploader()
//	{
//
//	}
//	TextureBatchUploader::~TextureBatchUploader()
//	{
//
//	}
//
//
//
//	// Public methods:
//	void TextureBatchUploader::EnqueueTexture(StagingBuffer* pStagingBuffer, Texture2d* pTexture2d)
//	{
//		m_pendingTextures.push_back({ std::unique_ptr<StagingBuffer>(pStagingBuffer), pTexture2d });
//	}
//	void TextureBatchUploader::UploadTextures()
//	{
//		const DeviceQueue& transferQueue = Context::logicalDevice.GetTransferQueue();
//		const DeviceQueue& graphicsQueue = Context::logicalDevice.GetGraphicsQueue();
//		if (transferQueue.queue != graphicsQueue.queue)
//		{
//			VkCommandBuffer transferCommandBuffer = SingleTimeCommand::BeginCommand(transferQueue);
//			VkCommandBuffer graphicsCommandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
//            NAME_VK_COMMAND_BUFFER(transferCommandBuffer, "Upload Textures: transfer");
//            NAME_VK_COMMAND_BUFFER(graphicsCommandBuffer, "Upload Textures: graphics");
//			for (auto& pendingTexture : m_pendingTextures)
//				pendingTexture.pTexture2d->RecordGpuCommands(transferCommandBuffer, graphicsCommandBuffer, pendingTexture.pStagingBuffer.get());
//            // Ember::ToDo: the linked command submission version is not working. Probably sync issues between the two queues.
//			// SingleTimeCommand::EndLinkedCommands(transferQueue, graphicsQueue, pipelineStage::transfer);
//			SingleTimeCommand::EndCommand(transferQueue);
//			SingleTimeCommand::EndCommand(graphicsQueue);
//		}
//		else
//		{
//			VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(graphicsQueue);
//            NAME_VK_COMMAND_BUFFER(commandBuffer, "Upload Textures: graphics + transfer");
//			for (auto& pendingTexture : m_pendingTextures)
//				pendingTexture.pTexture2d->RecordGpuCommands(commandBuffer, commandBuffer, pendingTexture.pStagingBuffer.get());
//			SingleTimeCommand::EndCommand(graphicsQueue);
//		}
//	}
//}