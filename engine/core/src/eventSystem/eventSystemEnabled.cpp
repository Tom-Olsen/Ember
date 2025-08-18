#include "eventSystemEnabled.h"
#include "dearImGui.h"
#include "logger.h"
#include "macros.h"
#include "sdlWindow.h"
#include "vulkanContext.h"



namespace emberEngine
{
    // Static members:
    bool EventSystem::s_isInitialized = false;
    std::unordered_map<SDL_Keycode, EventSystem::KeyState> EventSystem::s_keyStates;
    std::unordered_map<Uint8, EventSystem::MouseState> EventSystem::s_mouseButtonStates;
    float EventSystem::s_mouseX;
    float EventSystem::s_mouseY;
    float EventSystem::s_mouseScrollX;
    float EventSystem::s_mouseScrollY;
    bool EventSystem::s_windowResized;
    bool EventSystem::s_quit;



    // Initialization and cleanup:
    void EventSystem::Init()
    {
        if (s_isInitialized)
            return;
        s_isInitialized = true;

        s_keyStates = std::unordered_map<SDL_Keycode, KeyState>();
        s_mouseButtonStates = std::unordered_map<Uint8, MouseState>();
        s_mouseX = 0;
        s_mouseY = 0;
        s_windowResized = false;
        s_quit = false;

        #ifdef LOG_INITIALIZATION
        LOG_TRACE("EventSystem initialized.");
        #endif
    }
    void EventSystem::Clear()
    {

    }



    // Public methods:
    void EventSystem::ClearEvents()
    {
        // Reset released keys and transition pressed keys to held:
        for (auto& [key, state] : s_keyStates)
        {
            if (state == KeyState::down)
                state = KeyState::held;
            else if (state == KeyState::up)
                state = KeyState::none;
        }

        // Reset released mouse button and transition pressed button to held:
        for (auto& [button, state] : s_mouseButtonStates)
        {
            if (state == MouseState::down)
                state = MouseState::held;
            else if (state == MouseState::up)
                state = MouseState::none;
        }

        // Reset mouse scroll:
        s_mouseScrollX = 0;
        s_mouseScrollY = 0;

        // Reset event flags:
        s_windowResized = false;
        s_quit = false;
    }



    void EventSystem::ProcessEvent(const SDL_Event& event)
    {
        // Keayboard events:
        if (!DearImGui::WantCaptureKeyboard())
            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
                s_keyStates[event.key.key] = KeyState::down;
                break;
            case SDL_EVENT_KEY_UP:
                s_keyStates[event.key.key] = KeyState::up;
                break;
            default:
                break;
            }

        // Mouse events:
        if (!DearImGui::WantCaptureMouse())
            switch (event.type)
            {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                s_mouseButtonStates[event.button.button] = MouseState::down;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                s_mouseButtonStates[event.button.button] = MouseState::up;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                s_mouseX = event.motion.x;
                s_mouseY = event.motion.y;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                s_mouseScrollX = event.wheel.x;
                s_mouseScrollY = event.wheel.y;
                break;
            default:
                break;
            }

        // Other events:
        switch (event.type)
        {
        case SDL_EVENT_WINDOW_RESIZED:
            s_windowResized = true;
            break;
        case SDL_EVENT_QUIT:
            s_quit = true;
            break;
        default:
            break;
        }
    }



    bool EventSystem::AnyKeyDown()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::down)
                return true;
        return false;
    }
    bool EventSystem::AnyKeyUp()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::up)
                return true;
        return false;
    }
    bool EventSystem::AnyKeyHeld()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::held)
                return true;
        return false;
    }
    bool EventSystem::AnyMouseDown()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::down)
                return true;
        return false;
    }
    bool EventSystem::AnyMouseUp()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::up)
                return true;
        return false;
    }
    bool EventSystem::AnyMouseHeld()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::held)
                return true;
        return false;
    }
    bool EventSystem::KeyDown(SDL_Keycode key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::down;
    }
    bool EventSystem::KeyUp(SDL_Keycode key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::up;
    }
    bool EventSystem::KeyHeld(SDL_Keycode key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::held;
    }
    bool EventSystem::KeyDownOrHeld(SDL_Keycode key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && (it->second == KeyState::down || it->second == KeyState::held);
    }
    bool EventSystem::MouseDown(MouseButton button)
    {
        auto it = s_mouseButtonStates.find(static_cast<uint8_t>(button));
        return it != s_mouseButtonStates.end() && it->second == MouseState::down;
    }
    bool EventSystem::MouseUp(MouseButton button)
    {
        auto it = s_mouseButtonStates.find(static_cast<uint8_t>(button));
        return it != s_mouseButtonStates.end() && it->second == MouseState::up;
    }
    bool EventSystem::MouseHeld(MouseButton button)
    {
        auto it = s_mouseButtonStates.find(static_cast<uint8_t>(button));
        return it != s_mouseButtonStates.end() && it->second == MouseState::held;
    }
    bool EventSystem::MouseDown(uint8_t button)
    {
        auto it = s_mouseButtonStates.find(button);
        return it != s_mouseButtonStates.end() && it->second == MouseState::down;
    }
    bool EventSystem::MouseUp(uint8_t button)
    {
        auto it = s_mouseButtonStates.find(button);
        return it != s_mouseButtonStates.end() && it->second == MouseState::up;
    }
    bool EventSystem::MouseHeld(uint8_t button)
    {
        auto it = s_mouseButtonStates.find(button);
        return it != s_mouseButtonStates.end() && it->second == MouseState::held;
    }
    float EventSystem::MouseX()
    {
        return s_mouseX;
    }
    float EventSystem::MouseY()
    {
        return s_mouseY;
    }
    Float2 EventSystem::MousePos()
    {
        return Float2(s_mouseX, s_mouseY);
    }
    float EventSystem::MouseX01()
    {
        return s_mouseX / vulkanBackend::Context::pWindow->GetWidth();
    }
    float EventSystem::MouseY01()
    {
        return s_mouseY / vulkanBackend::Context::pWindow->GetHeight();
    }
    Float2 EventSystem::MousePos01()
    {
        VkExtent2D extend = vulkanBackend::Context::pWindow->GetExtent();
        return Float2(s_mouseX / extend.width, s_mouseY / extend.height);
    }
    float EventSystem::MouseScrollX()
    {
        return s_mouseScrollX;
    }
    float EventSystem::MouseScrollY()
    {
        return s_mouseScrollY;
    }
    bool EventSystem::WindowResized()
    {
        return s_windowResized;
    }
    bool EventSystem::Quit()
    {
        return s_quit;
    }
}