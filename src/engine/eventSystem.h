#ifndef __INCLUDE_GUARD_eventSystem_h__
#define __INCLUDE_GUARD_eventSystem_h__
#include "mathf.h"
#include <SDL3/SDL.h>
#include <unordered_map>



namespace emberEngine
{
    // Forward declarations:
    struct VulkanContext;



    class EventSystem
    {
    public: // Enums:
        enum class KeyState { none, down, up, held };
        enum class MouseState { none, down, up, held };
        enum class MouseButton { left = 1, middle = 2, right = 3 };

    private: // Members
        static VulkanContext* s_pContext;
        static bool s_isInitialized;
        static std::unordered_map<SDL_Keycode, KeyState> s_keyStates;
        static std::unordered_map<Uint8, MouseState> s_mouseButtonStates;
        static float s_mouseX;
        static float s_mouseY;
        static float s_mouseScrollX;
        static float s_mouseScrollY;
        static bool s_windowResized;
        static bool s_quit;

    public: // Methods:
        static void Init(VulkanContext* pContext);
        static void Clear();

        // Event handling:
        static void ClearEvents();
        static void ProcessEvent(const SDL_Event& event);

        // Event queries:
        static bool AnyKeyDown();
        static bool AnyKeyUp();
        static bool AnyKeyHeld();
        static bool AnyMouseDown();
        static bool AnyMouseUp();
        static bool AnyMouseHeld();
        static bool KeyDown(SDL_Keycode key);
        static bool KeyUp(SDL_Keycode key);
        static bool KeyHeld(SDL_Keycode key);
        static bool KeyDownOrHeld(SDL_Keycode key);
        static bool MouseDown(MouseButton button);
        static bool MouseUp(MouseButton button);
        static bool MouseHeld(MouseButton button);
        static bool MouseDown(uint8_t button);
        static bool MouseUp(uint8_t button);
        static bool MouseHeld(uint8_t button);
        static float MouseX();
        static float MouseY();
        static Float2 MousePos();
        static float MouseX01();
        static float MouseY01();
        static Float2 MousePos01();
        static float MouseScrollX();
        static float MouseScrollY();
        static bool WindowResized();
        static bool Quit();

    private: // Methods
        // Delete all constructors:
        EventSystem() = delete;
        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;
        ~EventSystem() = delete;
    };
}



#endif // __INCLUDE_GUARD_eventSystem_h__