#ifndef __INCLUDE_GUARD_eventSystemDisabled_h__
#define __INCLUDE_GUARD_eventSystemDisabled_h__
#include "emberMath.h"



// Forward declerations:
typedef int32_t SDL_Keycode;
typedef uint8_t Uint8;
struct SDL_Event;



namespace emberEngine
{
    class EventSystem
    {
    public: // Enums:
        enum class KeyState { none, down, up, held };
        enum class MouseState { none, down, up, held };
        enum class MouseButton { left = 1, middle = 2, right = 3 };

    public: // Methods:
        inline static void Init() {}
        inline static void Clear() {}

        // Event handling:
        inline static void ClearEvents() {}
        inline static void ProcessEvent(const SDL_Event& event) {}

        // Event queries:
        inline static bool AnyKeyDown() { return false; }
        inline static bool AnyKeyUp() { return false; }
        inline static bool AnyKeyHeld() { return false; }
        inline static bool AnyMouseDown() { return false; }
        inline static bool AnyMouseUp() { return false; }
        inline static bool AnyMouseHeld() { return false; }
        inline static bool KeyDown(SDL_Keycode key) { return false; }
        inline static bool KeyUp(SDL_Keycode key) { return false; }
        inline static bool KeyHeld(SDL_Keycode key) { return false; }
        inline static bool KeyDownOrHeld(SDL_Keycode key) { return false; }
        inline static bool MouseDown(MouseButton button) { return false; }
        inline static bool MouseUp(MouseButton button) { return false; }
        inline static bool MouseHeld(MouseButton button) { return false; }
        inline static bool MouseDown(uint8_t button) { return false; }
        inline static bool MouseUp(uint8_t button) { return false; }
        inline static bool MouseHeld(uint8_t button) { return false; }
        inline static float MouseX() { return 0.0f; }
        inline static float MouseY() { return 0.0f; }
        inline static Float2 MousePos() { return Float2::zero; }
        inline static float MouseX01() { return 0.0f; }
        inline static float MouseY01() { return 0.0f; }
        inline static Float2 MousePos01() { return Float2::zero; }
        inline static float MouseScrollX() { return 0.0f; }
        inline static float MouseScrollY() { return 0.0f; }
        inline static bool WindowResized() { return false; }
        inline static bool Quit() { return false; }

    private: // Methods
        // Delete all constructors:
        EventSystem() = delete;
        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;
        ~EventSystem() = delete;
    };
}



#endif // __INCLUDE_GUARD_eventSystemDisabled_h__