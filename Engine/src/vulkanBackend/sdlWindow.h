#ifndef __INCLUDE_GUARD_sdlWindow_h__
#define __INCLUDE_GUARD_sdlWindow_h__
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	namespace vulkanBackend
	{
		class SdlWindow
		{
		private: // Members:
			SDL_Window* m_pWindow;
			bool m_isMinimized;
			bool m_framebufferResized;

		public: // Methods:
			SdlWindow();
			void Init(uint16_t width, uint16_t height);
			~SdlWindow();

			bool HandleEvents();
			void AddSdlInstanceExtensions(std::vector<const char*>& instanceExtensions) const;

			// Getters:
			SDL_Window* const GetSDL_Window() const;
			bool GetIsMinimized() const;
			bool GetFramebufferResized() const;
			int GetWidth() const;
			int GetHeight() const;
			VkExtent2D GetExtent() const;

			// Setters:
			void SetFramebufferResized(bool value);
		};
	}
}



#endif // __INCLUDE_GUARD_sdlWindow_h__