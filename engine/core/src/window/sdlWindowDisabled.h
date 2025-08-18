#ifndef __INCLUDE_GUARD_sdlWindowDisabled_h__
#define __INCLUDE_GUARD_sdlWindowDisabled_h__
#include <vector>



namespace emberEngine
{
	namespace vulkanBackend
	{
		class SdlWindow
		{
		public: // Methods:
			inline SdlWindow() {}
			inline void Init(uint16_t width, uint16_t height) {}
			inline ~SdlWindow() {}

			inline bool HandleEvents() { return true; }
			inline void AddSdlInstanceExtensions(std::vector<const char*>& instanceExtensions) const {}

			// Getters:
			inline SDL_Window* const GetSDL_Window() const { return nullptr; }
			inline bool GetIsMinimized() const { return false; }
			inline bool GetFramebufferResized() const { return false; }
			inline int GetWidth() const { return 0; }
			inline int GetHeight() const { return 0; }
			inline VkExtent2D GetExtent() const { return { 0, 0 }; }

			// Setters:
			inline void SetFramebufferResized(bool value) {}
		};
	}
}



#endif // __INCLUDE_GUARD_sdlWindowDisabled_h__