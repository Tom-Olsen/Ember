#include "eventSystem.h"
#include "commonEvent.h"
#include "profiler.h"
#include "window.h"



namespace emberCore
{
    // Static members:
    bool EventSystem::s_isInitialized = false;
    std::unordered_map<emberCommon::Input::Key, EventSystem::KeyState> EventSystem::s_keyStates;
    std::unordered_map<emberCommon::Input::MouseButton, EventSystem::MouseState> EventSystem::s_mouseButtonStates;
    std::unordered_set<emberCommon::Input::MouseButton> EventSystem::s_guiCapturedMouseButtons;
    std::unordered_map<emberCommon::Input::Key, std::string> EventSystem::s_consumedKeys;
    std::unordered_map<emberCommon::Input::MouseButton, std::string> EventSystem::s_consumedMouseButtons;
    std::string EventSystem::s_emptyConsumer;
    std::string EventSystem::s_mouseScrollConsumer;
    bool EventSystem::s_mouseScrollConsumed;
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
        s_guiCapturedMouseButtons = std::unordered_set<emberCommon::Input::MouseButton>();
        s_consumedKeys = std::unordered_map<emberCommon::Input::Key, std::string>();
        s_consumedMouseButtons = std::unordered_map<emberCommon::Input::MouseButton, std::string>();
        s_emptyConsumer = "";
        s_mouseScrollConsumer = "";
        s_mouseScrollConsumed = false;
        s_mouseX = 0;
        s_mouseY = 0;
        s_mouseScrollX = 0;
        s_mouseScrollY = 0;
        s_windowWidth = Window::GetSize().x;
        s_windowHeight = Window::GetSize().y;
    }
    void EventSystem::Clear()
    {
        ClearInputState();
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
            if (!ShouldProcessEvent(event))
                continue;

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
                    if (event.windowID == Window::GetWindowID())
                    {
                        s_windowWidth = Window::GetSize().x;
                        s_windowHeight = Window::GetSize().y;
                    }
                    break;
                case emberCommon::EventType::WindowMinimized:
                    if (event.windowID == Window::GetWindowID())
                    {
                        s_windowWidth = 0;
                        s_windowHeight = 0;
                    }
                    break;
                case emberCommon::EventType::WindowRestored:
                    if (event.windowID == Window::GetWindowID())
                    {
                        s_windowWidth = Window::GetSize().x;
                        s_windowHeight = Window::GetSize().y;
                    }
                    break;
                case emberCommon::EventType::WindowFocusGained:
                    break;
                case emberCommon::EventType::WindowFocusLost:
                    ClearInputState();
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
        return true;	// application running.
    }



    // Event queries:
    bool EventSystem::AnyKeyDown()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::down && !KeyConsumed(key))
                return true;
        return false;
    }
    bool EventSystem::AnyKeyUp()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::up && !KeyConsumed(key))
                return true;
        return false;
    }
    bool EventSystem::AnyKeyHeld()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::held && !KeyConsumed(key))
                return true;
        return false;
    }
    bool EventSystem::AnyMouseDown()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::down && !MouseButtonConsumed(button))
                return true;
        return false;
    }
    bool EventSystem::AnyMouseUp()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::up && !MouseButtonConsumed(button))
                return true;
        return false;
    }
    bool EventSystem::AnyMouseHeld()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::held && !MouseButtonConsumed(button))
                return true;
        return false;
    }
    bool EventSystem::KeyDown(emberCommon::Input::Key key)
    {
        return KeyDownRaw(key) && !KeyConsumed(key);
    }
    bool EventSystem::KeyUp(emberCommon::Input::Key key)
    {
        return KeyUpRaw(key) && !KeyConsumed(key);
    }
    bool EventSystem::KeyHeld(emberCommon::Input::Key key)
    {
        return KeyHeldRaw(key) && !KeyConsumed(key);
    }
    bool EventSystem::KeyDownOrHeld(emberCommon::Input::Key key)
    {
        return KeyDownOrHeldRaw(key) && !KeyConsumed(key);
    }
    bool EventSystem::MouseDown(emberCommon::Input::MouseButton button)
    {
        return MouseDownRaw(button) && !MouseButtonConsumed(button);
    }
    bool EventSystem::MouseUp(emberCommon::Input::MouseButton button)
    {
        return MouseUpRaw(button) && !MouseButtonConsumed(button);
    }
    bool EventSystem::MouseHeld(emberCommon::Input::MouseButton button)
    {
        return MouseHeldRaw(button) && !MouseButtonConsumed(button);
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
        return MouseScrollConsumed() ? 0.0f : s_mouseScrollX;
    }
    float EventSystem::MouseScrollY()
    {
        return MouseScrollConsumed() ? 0.0f : s_mouseScrollY;
    }



    // Raw event queries:
    bool EventSystem::AnyKeyDownRaw()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::down)
                return true;
        return false;
    }
    bool EventSystem::AnyKeyUpRaw()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::up)
                return true;
        return false;
    }
    bool EventSystem::AnyKeyHeldRaw()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::held)
                return true;
        return false;
    }
    bool EventSystem::AnyMouseDownRaw()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::down)
                return true;
        return false;
    }
    bool EventSystem::AnyMouseUpRaw()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::up)
                return true;
        return false;
    }
    bool EventSystem::AnyMouseHeldRaw()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::held)
                return true;
        return false;
    }
    bool EventSystem::KeyDownRaw(emberCommon::Input::Key key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::down;
    }
    bool EventSystem::KeyUpRaw(emberCommon::Input::Key key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::up;
    }
    bool EventSystem::KeyHeldRaw(emberCommon::Input::Key key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && it->second == KeyState::held;
    }
    bool EventSystem::KeyDownOrHeldRaw(emberCommon::Input::Key key)
    {
        auto it = s_keyStates.find(key);
        return it != s_keyStates.end() && (it->second == KeyState::down || it->second == KeyState::held);
    }
    bool EventSystem::MouseDownRaw(emberCommon::Input::MouseButton button)
    {
        auto it = s_mouseButtonStates.find(button);
        return it != s_mouseButtonStates.end() && it->second == MouseState::down;
    }
    bool EventSystem::MouseUpRaw(emberCommon::Input::MouseButton button)
    {
        auto it = s_mouseButtonStates.find(button);
        return it != s_mouseButtonStates.end() && it->second == MouseState::up;
    }
    bool EventSystem::MouseHeldRaw(emberCommon::Input::MouseButton button)
    {
        auto it = s_mouseButtonStates.find(button);
        return it != s_mouseButtonStates.end() && it->second == MouseState::held;
    }
    float EventSystem::MouseScrollXRaw()
    {
        return s_mouseScrollX;
    }
    float EventSystem::MouseScrollYRaw()
    {
        return s_mouseScrollY;
    }
    void EventSystem::ConsumeKey(emberCommon::Input::Key key, const std::string& consumer)
    {
        if (key == emberCommon::Input::Key::Unknown)
            return;
        s_consumedKeys[key] = consumer;
    }
    void EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton button, const std::string& consumer)
    {
        if (button == emberCommon::Input::MouseButton::None)
            return;
        s_consumedMouseButtons[button] = consumer;
    }
    void EventSystem::ConsumeMouseScroll(const std::string& consumer)
    {
        s_mouseScrollConsumed = true;
        s_mouseScrollConsumer = consumer;
    }
    bool EventSystem::KeyConsumed(emberCommon::Input::Key key)
    {
        return s_consumedKeys.find(key) != s_consumedKeys.end();
    }
    bool EventSystem::MouseButtonConsumed(emberCommon::Input::MouseButton button)
    {
        return s_consumedMouseButtons.find(button) != s_consumedMouseButtons.end();
    }
    bool EventSystem::MouseScrollConsumed()
    {
        return s_mouseScrollConsumed;
    }
    const std::string& EventSystem::KeyConsumer(emberCommon::Input::Key key)
    {
        auto it = s_consumedKeys.find(key);
        return it == s_consumedKeys.end() ? s_emptyConsumer : it->second;
    }
    const std::string& EventSystem::MouseButtonConsumer(emberCommon::Input::MouseButton button)
    {
        auto it = s_consumedMouseButtons.find(button);
        return it == s_consumedMouseButtons.end() ? s_emptyConsumer : it->second;
    }
    const std::string& EventSystem::MouseScrollConsumer()
    {
        return s_mouseScrollConsumed ? s_mouseScrollConsumer : s_emptyConsumer;
    }



	// Private methods:
    void EventSystem::ClearInputState()
    {
        s_keyStates.clear();
        s_mouseButtonStates.clear();
        s_guiCapturedMouseButtons.clear();
        ClearConsumedEvents();
        s_mouseScrollX = 0;
        s_mouseScrollY = 0;
    }
    void EventSystem::ClearEvents()
    {
        ClearConsumedEvents();

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
    }
    void EventSystem::ClearConsumedEvents()
    {
        s_consumedKeys.clear();
        s_consumedMouseButtons.clear();
        s_mouseScrollConsumer.clear();
        s_mouseScrollConsumed = false;
    }
    bool EventSystem::ShouldProcessEvent(const emberCommon::Event& event)
    {
        switch (event.type)
        {
            case emberCommon::EventType::MouseMoved:
                return !AnyMouseButtonCapturedByGui() && !event.guiWantsMouse;
            case emberCommon::EventType::MouseButtonDown:
                if (event.guiWantsMouse)
                {
                    if (event.mouseButton != emberCommon::Input::MouseButton::None)
                        s_guiCapturedMouseButtons.insert(event.mouseButton);
                    return false;
                }
                return true;
            case emberCommon::EventType::MouseButtonUp:
                if (IsMouseButtonCapturedByGui(event.mouseButton))
                {
                    s_guiCapturedMouseButtons.erase(event.mouseButton);
                    return false;
                }
                return true;
            case emberCommon::EventType::MouseWheel:
                return !event.guiWantsMouse;
            case emberCommon::EventType::KeyDown:
            case emberCommon::EventType::KeyUp:
            case emberCommon::EventType::TextInput:
                return !event.guiWantsKeyboard;
            default:
                return true;
        }
    }
    bool EventSystem::IsMouseButtonCapturedByGui(emberCommon::Input::MouseButton button)
    {
        return s_guiCapturedMouseButtons.find(button) != s_guiCapturedMouseButtons.end();
    }
    bool EventSystem::AnyMouseButtonCapturedByGui()
    {
        return !s_guiCapturedMouseButtons.empty();
    }
}