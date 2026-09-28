#pragma once



// Forward declarations:
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace emberBackendInterface
{
	/// <summary>
	/// Vulkan-specific GUI coupling to inject vulkan commands from other backends (imgui).
	/// </summary>
	class IVulkanGui
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IVulkanGui() = default;

		// Rendering:
		virtual void Render(VkCommandBuffer commandBuffer) = 0;
	};
}