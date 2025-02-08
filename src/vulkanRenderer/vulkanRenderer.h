#ifndef __INCLUDE_GUARD_vulkanRenderer_h__
#define __INCLUDE_GUARD_vulkanRenderer_h__
#include <vulkan/vulkan.h>
#include <array>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class Mesh;
	class MeshRenderer;
	class Scene;
	struct VulkanContext;
	class VulkanCommand;



	class VulkanRenderer
	{
	private: // Members:
		VulkanContext* m_pContext;

		// Render resources:
		std::vector<VulkanCommand> m_shadowCommands;
		std::vector<VulkanCommand> m_shadingCommands;

		// Sync objects:
		std::vector<VkFence> m_fences;
		std::vector<VkSemaphore> m_acquireSemaphores;
		std::vector<VkSemaphore> m_shadowToShadingSemaphores;
		std::vector<VkSemaphore> m_releaseSemaphores;

		// Render management:
		uint32_t m_imageIndex;
		bool m_rebuildSwapchain;
		std::array<std::vector<MeshRenderer*>*, 2> m_pMeshRendererGroups;

	public: // Methods:
		VulkanRenderer(VulkanContext* pContext);
		~VulkanRenderer();
		bool RenderFrame(Scene* pScene);
		const VulkanContext* const GetContext() const;

	private: // Methods:
		void RebuildSwapchain();
		bool AcquireImage();
		void SetMeshRendererGroups(Scene* pScene);
		void RecordShadowCommandBuffer(Scene* pScene);
		void RecordShadingCommandBuffer(Scene* pScene);
		void SubmitCommandBuffers();
		bool PresentImage();
		void SetViewportAndScissor(VkCommandBuffer& commandBuffer);
		void CreateFences();
		void CreateSemaphores();
		void DestroyFences();
		void DestroySemaphores();
	};
}



#endif // __INCLUDE_GUARD_vulkanRenderer_h__