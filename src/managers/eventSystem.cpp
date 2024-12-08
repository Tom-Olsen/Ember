#include "eventSystem.h"
#include "logger.h"



// Static members:
VulkanContext* EventSystem::context = nullptr;
bool EventSystem::isInitialized = false;
std::unordered_map<SDL_Keycode, EventSystem::KeyState> EventSystem::keyStates;
std::unordered_map<Uint8, EventSystem::MouseState> EventSystem::mouseButtonStates;
float EventSystem::mouseX;
float EventSystem::mouseY;
float EventSystem::mouseScrollX;
float EventSystem::mouseScrollY;
bool EventSystem::windowResized;
bool EventSystem::quit;



// Initialization and cleanup:
void EventSystem::Init(VulkanContext* context)
{
    if (isInitialized)
        return;
    EventSystem::context = context;
    EventSystem::isInitialized = true;

	EventSystem::keyStates = std::unordered_map<SDL_Keycode, KeyState>();
	EventSystem::mouseButtonStates = std::unordered_map<Uint8, MouseState>();
	EventSystem::mouseX = 0;
	EventSystem::mouseY = 0;
	EventSystem::windowResized = false;
	EventSystem::quit = false;
}
void EventSystem::Clear()
{

}



// Public methods:
void EventSystem::ClearEvents()
{
	// Reset released keys and transition pressed keys to held:
    for (auto& [key, state] : keyStates)
    {
        if (state == KeyState::down)
            state = KeyState::held;
        else if (state == KeyState::up) 
            state = KeyState::none;
    }

	// Reset released mouse button and transition pressed button to held:
    for (auto& [button, state] : mouseButtonStates)
    {
        if (state == MouseState::down)
            state = MouseState::held;
        else if (state == MouseState::up)
            state = MouseState::none;
    }

    // Reset mouse scroll:
	mouseScrollX = 0;
	mouseScrollY = 0;

	// Reset event flags:
    windowResized= false;
    quit = false;
}



void EventSystem::ProcessEvent(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_EVENT_KEY_DOWN:
        keyStates[event.key.key] = KeyState::down;
        break;
    case SDL_EVENT_KEY_UP:
        keyStates[event.key.key] = KeyState::up;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        mouseButtonStates[event.button.button] = MouseState::down;
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        mouseButtonStates[event.button.button] = MouseState::up;
        break;
    case SDL_EVENT_MOUSE_MOTION:
        mouseX = event.motion.x;
        mouseY = event.motion.y;
        break;
	case SDL_EVENT_MOUSE_WHEEL:
		mouseScrollX = event.wheel.x;
		mouseScrollY = event.wheel.y;
		break;
    case SDL_EVENT_WINDOW_RESIZED:
        windowResized = true;
        break;
    case SDL_EVENT_QUIT:
        quit = true;
        break;
    default:
        break;
    }
}



bool EventSystem::KeyDown(SDL_Keycode key)
{
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second == KeyState::down;
}
bool EventSystem::KeyUp(SDL_Keycode key)
{
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second == KeyState::up;
}
bool EventSystem::KeyHeld(SDL_Keycode key)
{
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second == KeyState::held;
}
bool EventSystem::KeyDownOrHeld(SDL_Keycode key)
{
	auto it = keyStates.find(key);
	return it != keyStates.end() && (it->second == KeyState::down || it->second == KeyState::held);
}
bool EventSystem::MouseDown(MouseButton button)
{
    auto it = mouseButtonStates.find(static_cast<uint8_t>(button));
    return it != mouseButtonStates.end() && it->second == MouseState::down;
}
bool EventSystem::MouseUp(MouseButton button)
{
    auto it = mouseButtonStates.find(static_cast<uint8_t>(button));
    return it != mouseButtonStates.end() && it->second == MouseState::up;
}
bool EventSystem::MouseHeld(MouseButton button)
{
    auto it = mouseButtonStates.find(static_cast<uint8_t>(button));
    return it != mouseButtonStates.end() && it->second == MouseState::held;
}
bool EventSystem::MouseDown(uint8_t button)
{
    auto it = mouseButtonStates.find(button);
    return it != mouseButtonStates.end() && it->second == MouseState::down;
}
bool EventSystem::MouseUp(uint8_t button)
{
    auto it = mouseButtonStates.find(button);
    return it != mouseButtonStates.end() && it->second == MouseState::up;
}
bool EventSystem::MouseHeld(uint8_t button)
{
	auto it = mouseButtonStates.find(button);
	return it != mouseButtonStates.end() && it->second == MouseState::held;
}
float EventSystem::MouseX()
{
	return mouseX;
}
float EventSystem::MouseY()
{
	return mouseY;
}
Float2 EventSystem::MousePos()
{
	return Float2(mouseX, mouseY);
}
float EventSystem::MouseX01()
{
	return mouseX / context->pWindow->GetWidth();
}
float EventSystem::MouseY01()
{
	return mouseY / context->pWindow->GetHeight();
}
Float2 EventSystem::MousePos01()
{
	VkExtent2D extend = context->pWindow->GetExtent();
	return Float2(mouseX / extend.width, mouseY / extend.height);
}
float EventSystem::MouseScrollX()
{
	return mouseScrollX;
}
float EventSystem::MouseScrollY()
{
	return mouseScrollY;
}
bool EventSystem::WindowResized()
{
    return windowResized;
}
bool EventSystem::Quit()
{
    return quit;
}