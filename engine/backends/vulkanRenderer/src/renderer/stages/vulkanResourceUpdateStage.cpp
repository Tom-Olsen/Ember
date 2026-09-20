#include "vulkanResourceUpdateStage.h"
#include "profiler.h"
#include "vulkanCommandPool.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameRenderData.h"
#include "vulkanFrameResources.h"
#include "vulkanMacros.h"
#include "vulkanMesh.h"



namespace vulkanRendererBackend
{
	// Public methods:
	void ResourceUpdateStage::Record(const FrameContext& frameContext)
	{
		PROFILE_FUNCTION();

		// Prepare meshes to update:
		std::vector<Mesh*>& meshUpdates = frameContext.frameRenderData.meshUpdates;

		// Prepare command recording:
		CommandPool& commandPool = frameContext.resources.GetCommandPool(RenderStage::resourceUpdate);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record resource update commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			for (Mesh* mesh : meshUpdates)
				mesh->RecordUpdateCommand(commandBuffer, frameContext.frameExecutionData.frameIndex);
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Clear only mesh updates of current frame:
		meshUpdates.clear();
	}
}