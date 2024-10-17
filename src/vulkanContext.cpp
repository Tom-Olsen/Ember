#include "vulkanContext.h"



// Constructor:
VulkanContext::VulkanContext(SdlWindow* window, VulkanInstance* instance, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, VulkanLogicalDevice* logicalDevice, VulkanMemoryAllocator* allocator, VulkanDescriptorPool* descriptorPool, uint32_t framesInFlight)
{
	this->window = window;
	this->instance = instance;
	this->physicalDevice = physicalDevice;
	this->surface = surface;
	this->logicalDevice = logicalDevice;
	this->allocator = allocator;
	this->descriptorPool = descriptorPool;
	this->framesInFlight = framesInFlight;
	this->frameIndex = 0;
}



// Destructor:
VulkanContext::~VulkanContext()
{

}



// Getters:
SDL_Window* VulkanContext::Window()
{
	return window->window;
}
VkInstance& VulkanContext::Instance()
{
	return instance->instance;
}
VkPhysicalDevice& VulkanContext::PhysicalDevice()
{
	return physicalDevice->device;
}
VkSurfaceKHR& VulkanContext::Surface()
{
	return surface->surface;
}
VkDevice& VulkanContext::LogicalDevice()
{
	return logicalDevice->device;
}
VmaAllocator& VulkanContext::Allocator()
{
	return allocator->allocator;
}
VkDescriptorPool& VulkanContext::DescriptorPool()
{
	return descriptorPool->descriptorPool;
}



// Public frame logic:
void VulkanContext::UpdateFrameIndex()
{
	frameIndex = (frameIndex + 1) % framesInFlight;
}
void VulkanContext::ResetFrameIndex()
{
	frameIndex = 0;
}