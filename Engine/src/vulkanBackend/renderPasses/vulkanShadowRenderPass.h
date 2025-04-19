#ifndef __INCLUDE_GUARD_vulkanShadowRenderPass_h__
#define __INCLUDE_GUARD_vulkanShadowRenderPass_h__
#include "vulkanRenderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class DepthTexture2dArray;



	namespace vulkanBackend
	{
		// Forward declarations:
		class VmaImage;



		/// <summary>
		/// Shadow render pass.
		/// Executed once for every light source.
		/// </summary>
		class ShadowRenderPass : public RenderPass
		{
		private: // Members:
			std::unique_ptr<DepthTexture2dArray> m_shadowMaps;

		public: // Members:
			static VkFormat s_shadowMapFormat;
			static uint32_t s_layerCount;

		public: // Methods:
			ShadowRenderPass();
			~ShadowRenderPass();
			DepthTexture2dArray* const GetShadowMaps() const;

		private: // Methods:
			void CreateRenderpass();
			void CreateFramebuffers();
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanShadowRenderPass_h__