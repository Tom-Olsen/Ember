#include "eventSystem.h"
#include "commonEvent.h"
#include "profiler.h"
#include "window.h"



namespace emberEngine
{
    // Static members:
    bool EventSystem::s_isInitialized = false;
    std::unordered_map<emberCommon::Input::Key, EventSystem::KeyState> EventSystem::s_keyStates;
    std::unordered_map<emberCommon::Input::MouseButton, EventSystem::MouseState> EventSystem::s_mouseButtonStates;
    float EventSystem::s_mouseX;
    float EventSystem::s_mouseY;
    float EventSystem::s_mouseScrollX;
    float EventSystem::s_mouseScrollY;
    int EventSystem::s_windowWidth;
    int EventSystem::s_windowHeight;



    // Initialization/Cleanup:
    void EventSystem::Init()
    {
        if (s_isInitialized)
            return;
        s_isInitialized = true;

        s_keyStates = std::unordered_map<emberCommon::Input::Key, KeyState>();
        s_mouseButtonStates = std::unordered_map<emberCommon::Input::MouseButton, MouseState>();
        s_mouseX = 0;
        s_mouseY = 0;
        s_windowWidth = Window::GetSize().x;
        s_windowHeight = Window::GetSize().y;
    }
    void EventSystem::Clear()
    {
        ClearEvents();
        s_keyStates.clear();
        s_mouseButtonStates.clear();
        s_isInitialized = false;
    }



    // Public methods:
    bool EventSystem::ProcessEvents()
    {
        PROFILE_FUNCTION();
		ClearEvents();  // reset and transition previous event states.
        
        // Poll events:
        std::vector<emberCommon::Event> events = Window::PollEvents();
        for (const emberCommon::Event& event : events)
        {
            switch (event.type)
            {
                // System / Window:
                case emberCommon::EventType::Quit:
                    return false;
                    break;
                case emberCommon::EventType::WindowClose:
                    if (event.windowID == Window::GetWindowID()) // check for main window.
                    {
                        s_windowWidth = 0;
                        s_windowHeight = 0;
                        return false;
                    }
                    break;
                case emberCommon::EventType::WindowResized:
                    s_windowWidth = Window::GetSize().x;
                    s_windowHeight = Window::GetSize().y;
                    break;
                case emberCommon::EventType::WindowMinimized:
                    s_windowWidth = 0;
                    s_windowHeight = 0;
                    break;
                case emberCommon::EventType::WindowRestored:
                    s_windowWidth = Window::GetSize().x;
                    s_windowHeight = Window::GetSize().y;
                    break;
                case emberCommon::EventType::WindowFocusGained:
                    break;
                case emberCommon::EventType::WindowFocusLost:
                    break;

                // Keyboard events:
                case emberCommon::EventType::KeyDown:
                    s_keyStates[event.key] = KeyState::down;
                    break;
                case emberCommon::EventType::KeyUp:
                    s_keyStates[event.key] = KeyState::up;
                    break;
                case emberCommon::EventType::TextInput:
                    break;

                    // Mouse events:
                case emberCommon::EventType::MouseMoved:
                    s_mouseX = event.mousePosX;
                    s_mouseY = event.mousePosY;
                    break;
                case emberCommon::EventType::MouseButtonDown:
                    s_mouseButtonStates[event.mouseButton] = MouseState::down;
                    break;
                case emberCommon::EventType::MouseButtonUp:
                    s_mouseButtonStates[event.mouseButton] = MouseState::up;
                    break;
                case emberCommon::EventType::MouseWheel:
                    s_mouseScrollX = event.mouseWheelX;
                    s_mouseScrollY = event.mouseWheelY;
                    break;

                // Controller events:
                case emberCommon::EventType::ControllerConnected:
                    // Ember::ToDo: handle controller connection.
                    break;
                case emberCommon::EventType::ControllerDisconnected:
                    // Ember::ToDo: handle controller disconnection.
                    break;
                case emberCommon::EventType::ControllerButtonDown:
                    // Ember::ToDo: handle controller button down.
                    break;
                case emberCommon::EventType::ControllerButtonUp:
                    // Ember::ToDo: handle controller button up.
                    break;
                case emberCommon::EventType::ControllerAxisMotion:
                    // Ember::ToDo: handle controller axis motion.
                    break;

                default:
                    break;
            }
        }
        
        return true;	// application running
    }


    // Event queries:
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
    bool EventSystem::KeyDown(emberCommon::Input::Key key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::down;
    }
    bool EventSystem::KeyUp(emberCommon::Input::Key key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::up;
    }
    bool EventSystem::KeyHeld(emberCommon::Input::Key key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::held;
    }
    bool EventSystem::KeyDownOrHeld(emberCommon::Input::Key key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && (it->second == KeyState::down || it->second == KeyState::held);
    }
    bool EventSystem::MouseDown(emberCommon::Input::MouseButton button)
    {
        auto it = s_mouseButtonStates.find(button);
        return it != s_mouseButtonStates.end() && it->second == MouseState::down;
    }
    bool EventSystem::MouseUp(emberCommon::Input::MouseButton button)
    {
        auto it = s_mouseButtonStates.find(button);
        return it != s_mouseButtonStates.end() && it->second == MouseState::up;
    }
    bool EventSystem::MouseHeld(emberCommon::Input::MouseButton button)
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
        return s_windowWidth == 0 ? 0 : s_mouseX / s_windowWidth;
    }
    float EventSystem::MouseY01()
    {
        return s_windowHeight == 0 ? 0 : s_mouseY / s_windowHeight;
    }
    Float2 EventSystem::MousePos01()
    {
        return Float2(MouseX01(), MouseY01());
    }
    float EventSystem::MouseScrollX()
    {
        return s_mouseScrollX;
    }
    float EventSystem::MouseScrollY()
    {
        return s_mouseScrollY;
    }



	// Private methods:
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
        //s_windowResized = false;
        //s_quit = false;
    }
}