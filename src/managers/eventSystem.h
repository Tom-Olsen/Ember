#ifndef __INCLUDE_GUARD_eventSystem_h__
#define __INCLUDE_GUARD_eventSystem_h__
#include <SDL3/SDL.h>
#include <unordered_map>
#include "vulkanContext.h"
#include "mathf.h"



class EventSystem
{
public: // Enums:
    enum class KeyState { none, down, up, held };
    enum class MouseState { none, down, up, held };
    enum class MouseButton { left = 1, middle = 2, right = 3};

private: // Members
    static VulkanContext* context;
    static bool isInitialized;
    static std::unordered_map<SDL_Keycode, KeyState> keyStates;
    static std::unordered_map<Uint8, MouseState> mouseButtonStates;
    static float mouseX;
    static float mouseY;
	static float mouseScrollX;
	static float mouseScrollY;
    static bool windowResized;
    static bool quit;

public: // Methods:
    static void Init(VulkanContext* context);
    static void Clear();

	// Event handling:
    static void ClearEvents();
    static void ProcessEvent(const SDL_Event& event);

	// Event queries:
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



#endif // __INCLUDE_GUARD_eventSystem_h__