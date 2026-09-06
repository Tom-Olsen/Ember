#include "vulkanFrameResources.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	FrameResources::FrameResources(int secondaryBufferCount)
	{
		commandPools.reserve(static_cast<size_t>(RenderStage::stageCount));
		for (int renderStage = 0; renderStage < static_cast<int>(RenderStage::stageCount); renderStage++)
			commandPools.emplace_back(secondaryBufferCount, Context::GetLogicalDevice()->GetGraphicsQueue());
	}



	// Getters:
	CommandPool& FrameResources::GetCommandPool(RenderStage renderStage)
	{
		return GetCommandPool(static_cast<int>(renderStage));
	}
	CommandPool& FrameResources::GetCommandPool(int renderStage)
	{
		if (renderStage < 0 || renderStage >= static_cast<int>(RenderStage::stageCount))
			throw std::out_of_range("FrameResources::GetCommandPool(...) failed. renderStage is out of range.");
		return commandPools[static_cast<size_t>(renderStage)];
	}
}