#include "vulkanContext.h"
#include "dearImGui.h"
#include "vulkanMacros.h"



// Constructor:
VulkanContext::VulkanContext(uint32_t framesInFlight, VkSampleCountFlagBits msaaSamples)
{
	this->framesInFlight = framesInFlight;
	this->frameIndex = 0;

	// Window:
	pWindow = std::make_unique<SdlWindow>();

	// Get instance extensions:
	std::vector<const char*> instanceExtensions;
	#if defined(VALIDATION_LAYERS_ACTIVE)
	instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
	#endif
	pWindow->AddSdlInstanceExtensions(instanceExtensions);		// sdl instance extensions
	DearImGui::AddImGuiInstanceExtensions(instanceExtensions);	// add instance extensions for docking feature

	// and more ...

	// Get device extensions:
	std::vector<const char*> deviceExtensions;
	deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	deviceExtensions.emplace_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
	// and more ...

	// Create vulkan context:
	pInstance = std::make_unique<VulkanInstance>(instanceExtensions);
	pPhysicalDevice = std::make_unique<VulkanPhysicalDevice>(pInstance.get());
	pSurface = std::make_unique<VulkanSurface>(pInstance.get(), pPhysicalDevice.get(), pWindow.get());
	pLogicalDevice = std::make_unique<VulkanLogicalDevice>(pPhysicalDevice.get(), pSurface.get(), deviceExtensions);
	pAllocator = std::make_unique<VulkanMemoryAllocator>(pInstance.get(), pLogicalDevice.get(), pPhysicalDevice.get());
	pDescriptorPool = std::make_unique<VulkanDescriptorPool>(pLogicalDevice.get());
	pSwapchain = std::make_unique<VulkanSwapchain>(pLogicalDevice.get(), pSurface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	this->msaaSamples = std::min(msaaSamples, pPhysicalDevice->GetMaxMsaaSamples());
}



// Destructor:
VulkanContext::~VulkanContext()
{
	WaitDeviceIdle();
}



// Getters:
SDL_Window* const VulkanContext::GetSDL_Window() const
{
	return pWindow->GetSDL_Window();
}
const VkInstance& VulkanContext::GetVkInstance() const
{
	return pInstance->GetVkInstance();
}
const VkPhysicalDevice& VulkanContext::GetVkPhysicalDevice() const
{
	return pPhysicalDevice->GetVkPhysicalDevice();
}
const VkSurfaceKHR& VulkanContext::GetVkSurfaceKHR() const
{
	return pSurface->GetVkSurfaceKHR();
}
const VkDevice& VulkanContext::GetVkDevice() const
{
	return pLogicalDevice->GetVkDevice();
}
const VmaAllocator& VulkanContext::GetVmaAllocator() const
{
	return pAllocator->GetVmaAllocator();
}
const VkDescriptorPool& VulkanContext::GetVkDescriptorPool() const
{
	return pDescriptorPool->GetVkDescriptorPool();
}
const VkSwapchainKHR& VulkanContext::GetVkSwapchainKHR() const
{
	return pSwapchain->GetVkSwapchainKHR();
}
bool VulkanContext::DepthClampEnabled() const
{
	return pPhysicalDevice->SupportsDepthClamp();
}
bool VulkanContext::DepthBiasEnabled() const
{
	return pPhysicalDevice->SupportsDepthBias();
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
void VulkanContext::WaitDeviceIdle()
{
	VKA(vkDeviceWaitIdle(pLogicalDevice->GetVkDevice()));
}