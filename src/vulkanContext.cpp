#include "vulkanContext.h"



// Constructor:
VulkanContext::VulkanContext(SdlWindow* window, VulkanInstance* instance, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, VulkanLogicalDevice* logicalDevice, VulkanMemoryAllocator* allocator, uint32_t framesInFlight)
{
	this->window = window;
	this->instance = instance;
	this->physicalDevice = physicalDevice;
	this->surface = surface;
	this->logicalDevice = logicalDevice;
	this->allocator = allocator;
	this->framesInFlight = framesInFlight;
}



// Destructor:
VulkanContext::~VulkanContext()
{

}



// Public methods:
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