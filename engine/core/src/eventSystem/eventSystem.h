#ifndef __INCLUDE_GUARD_eventSystem_h__
#define __INCLUDE_GUARD_eventSystem_h__
#include "emberMath.h"
#include "input.h"
#include <unordered_map>



namespace emberEngine
{
    class EventSystem
    {
    public: // Enums:
        enum class KeyState { none, down, up, held };
        enum class MouseState { none, down, up, held };

    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<Input::Key, KeyState> s_keyStates;
        static std::unordered_map<Input::MouseButton, MouseState> s_mouseButtonStates;
        static float s_mouseX;
        static float s_mouseY;
        static float s_mouseScrollX;
        static float s_mouseScrollY;
		static int s_windowWidth;
		static int s_windowHeight;

    public: // Methods:
        static void Init();
        static void Clear();

        // Event handling:
        static bool ProcessEvents();

        // Event queries:
        static bool AnyKeyDown();
        static bool AnyKeyUp();
        static bool AnyKeyHeld();
        static bool AnyMouseDown();
        static bool AnyMouseUp();
        static bool AnyMouseHeld();
        static bool KeyDown(Input::Key key);
        static bool KeyUp(Input::Key key);
        static bool KeyHeld(Input::Key key);
        static bool KeyDownOrHeld(Input::Key key);
        static bool MouseDown(Input::MouseButton button);
        static bool MouseUp(Input::MouseButton button);
        static bool MouseHeld(Input::MouseButton button);
        static float MouseX();
        static float MouseY();
        static Float2 MousePos();
        static float MouseX01();
        static float MouseY01();
        static Float2 MousePos01();
        static float MouseScrollX();
        static float MouseScrollY();

    private: // Methods
        static void ClearEvents();

        // Delete all constructors:
        EventSystem() = delete;
        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;
        ~EventSystem() = delete;
    };
}



#endif // __INCLUDE_GUARD_eventSystem_h__