#include "eventSystem.h"
#include "commonEvent.h"
#include "profiler.h"
#include "window.h"
#include <algorithm>



namespace emberCore
{
    // Static members:
    bool EventSystem::s_isInitialized = false;
    // Input states:
    std::unordered_map<emberCommon::Input::Key, EventSystem::KeyState> EventSystem::s_keyStates;
    std::unordered_map<emberCommon::Input::MouseButton, EventSystem::MouseState> EventSystem::s_mouseButtonStates;
    std::unordered_map<uint32_t, std::unordered_map<emberCommon::Input::ControllerButton, EventSystem::ControllerButtonState>> EventSystem::s_controllerButtonStates;
    std::unordered_map<uint32_t, std::unordered_map<emberCommon::Input::ControllerAxis, float>> EventSystem::s_controllerAxisStates;
    std::unordered_set<uint32_t> EventSystem::s_connectedControllers;
    // MouseButton lock/unlock:
    std::unordered_map<emberCommon::Input::MouseButton, EventSystem::Consumer> EventSystem::s_lockedMouseButtons;
    std::unordered_set<emberCommon::Input::MouseButton> EventSystem::s_mouseButtonLocksPendingUnlock;
    // Input consumption:
    std::unordered_map<emberCommon::Input::Key, EventSystem::Consumer> EventSystem::s_consumedKeys;
    std::unordered_map<emberCommon::Input::MouseButton, EventSystem::Consumer> EventSystem::s_consumedMouseButtons;
    std::unordered_map<uint32_t, std::unordered_map<emberCommon::Input::ControllerButton, EventSystem::Consumer>> EventSystem::s_consumedControllerButtons;
    std::unordered_map<uint32_t, std::unordered_map<emberCommon::Input::ControllerAxis, EventSystem::Consumer>> EventSystem::s_consumedControllerAxes;
    EventSystem::Consumer EventSystem::s_currentConsumer;
    EventSystem::Consumer EventSystem::s_keyboardLockConsumer;
    EventSystem::Consumer EventSystem::s_mouseScrollConsumer;
    // Basics:
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

        // Input states:
        s_keyStates = std::unordered_map<emberCommon::Input::Key, KeyState>();
        s_mouseButtonStates = std::unordered_map<emberCommon::Input::MouseButton, MouseState>();
        s_controllerButtonStates = std::unordered_map<uint32_t, std::unordered_map<emberCommon::Input::ControllerButton, ControllerButtonState>>();
        s_controllerAxisStates = std::unordered_map<uint32_t, std::unordered_map<emberCommon::Input::ControllerAxis, float>>();
        s_connectedControllers = std::unordered_set<uint32_t>();
        // MouseButton lock/unlock:
        s_lockedMouseButtons = std::unordered_map<emberCommon::Input::MouseButton, Consumer>();
        s_mouseButtonLocksPendingUnlock = std::unordered_set<emberCommon::Input::MouseButton>();
        // Input consumption:
        s_consumedKeys = std::unordered_map<emberCommon::Input::Key, Consumer>();
        s_consumedMouseButtons = std::unordered_map<emberCommon::Input::MouseButton, Consumer>();
        s_consumedControllerButtons = std::unordered_map<uint32_t, std::unordered_map<emberCommon::Input::ControllerButton, Consumer>>();
        s_consumedControllerAxes = std::unordered_map<uint32_t, std::unordered_map<emberCommon::Input::ControllerAxis, Consumer>>();
        s_currentConsumer = Consumer::none;
        s_keyboardLockConsumer = Consumer::none;
        s_mouseScrollConsumer = Consumer::none;
        // Basics:
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
        s_connectedControllers.clear();
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
            // Events are also forwarded to gui. This checks if gui has claimed any ownership:
            if (!ProcessGuiEventFilter(event))
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
                    if (MouseButtonLocked(event.mouseButton))
                        s_mouseButtonLocksPendingUnlock.insert(event.mouseButton);
                    break;
                case emberCommon::EventType::MouseWheel:
                    s_mouseScrollX = event.mouseWheelX;
                    s_mouseScrollY = event.mouseWheelY;
                    break;

                // Controller events:
                case emberCommon::EventType::ControllerConnected:
                    s_connectedControllers.insert(event.controllerId);
                    s_controllerButtonStates.try_emplace(event.controllerId);
                    s_controllerAxisStates.try_emplace(event.controllerId);
                    break;
                case emberCommon::EventType::ControllerDisconnected:
                    s_connectedControllers.erase(event.controllerId);
                    s_controllerButtonStates.erase(event.controllerId);
                    s_controllerAxisStates.erase(event.controllerId);
                    s_consumedControllerButtons.erase(event.controllerId);
                    s_consumedControllerAxes.erase(event.controllerId);
                    break;
                case emberCommon::EventType::ControllerButtonDown:
                    if (event.controllerButton != emberCommon::Input::ControllerButton::None && ControllerConnected(event.controllerId))
                        s_controllerButtonStates[event.controllerId][event.controllerButton] = ControllerButtonState::down;
                    break;
                case emberCommon::EventType::ControllerButtonUp:
                    if (event.controllerButton != emberCommon::Input::ControllerButton::None && ControllerConnected(event.controllerId))
                        s_controllerButtonStates[event.controllerId][event.controllerButton] = ControllerButtonState::up;
                    break;
                case emberCommon::EventType::ControllerAxisMotion:
                    if (event.controllerAxis != emberCommon::Input::ControllerAxis::None && ControllerConnected(event.controllerId))
                        s_controllerAxisStates[event.controllerId][event.controllerAxis] = event.controllerAxisValue;
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
            if (state == KeyState::down && !KeyBlocked(key))
                return true;
        return false;
    }
    bool EventSystem::AnyKeyUp()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::up && !KeyBlocked(key))
                return true;
        return false;
    }
    bool EventSystem::AnyKeyHeld()
    {
        for (auto& [key, state] : s_keyStates)
            if (state == KeyState::held && !KeyBlocked(key))
                return true;
        return false;
    }
    bool EventSystem::AnyMouseDown()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::down && !MouseButtonBlocked(button))
                return true;
        return false;
    }
    bool EventSystem::AnyMouseUp()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::up && !MouseButtonBlocked(button))
                return true;
        return false;
    }
    bool EventSystem::AnyMouseHeld()
    {
        for (auto& [button, state] : s_mouseButtonStates)
            if (state == MouseState::held && !MouseButtonBlocked(button))
                return true;
        return false;
    }
    bool EventSystem::KeyDown(emberCommon::Input::Key key)
    {
        return KeyDownRaw(key) && !KeyBlocked(key);
    }
    bool EventSystem::KeyUp(emberCommon::Input::Key key)
    {
        return KeyUpRaw(key) && !KeyBlocked(key);
    }
    bool EventSystem::KeyHeld(emberCommon::Input::Key key)
    {
        return KeyHeldRaw(key) && !KeyBlocked(key);
    }
    bool EventSystem::KeyDownOrHeld(emberCommon::Input::Key key)
    {
        return KeyDownOrHeldRaw(key) && !KeyBlocked(key);
    }
    bool EventSystem::MouseDown(emberCommon::Input::MouseButton button)
    {
        return MouseDownRaw(button) && !MouseButtonBlocked(button);
    }
    bool EventSystem::MouseUp(emberCommon::Input::MouseButton button)
    {
        return MouseUpRaw(button) && !MouseButtonBlocked(button);
    }
    bool EventSystem::MouseHeld(emberCommon::Input::MouseButton button)
    {
        return MouseHeldRaw(button) && !MouseButtonBlocked(button);
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
        return MouseScrollBlocked() ? 0.0f : s_mouseScrollX;
    }
    float EventSystem::MouseScrollY()
    {
        return MouseScrollBlocked() ? 0.0f : s_mouseScrollY;
    }
    bool EventSystem::ControllerButtonDown(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        return ControllerButtonDownRaw(controllerId, button) && !ControllerButtonBlocked(controllerId, button);
    }
    bool EventSystem::ControllerButtonUp(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        return ControllerButtonUpRaw(controllerId, button) && !ControllerButtonBlocked(controllerId, button);
    }
    bool EventSystem::ControllerButtonHeld(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        return ControllerButtonHeldRaw(controllerId, button) && !ControllerButtonBlocked(controllerId, button);
    }
    bool EventSystem::ControllerButtonDownOrHeld(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        return ControllerButtonDownOrHeldRaw(controllerId, button) && !ControllerButtonBlocked(controllerId, button);
    }
    float EventSystem::ControllerAxis(uint32_t controllerId, emberCommon::Input::ControllerAxis axis)
    {
        return ControllerAxisBlocked(controllerId, axis) ? 0.0f : ControllerAxisRaw(controllerId, axis);
    }
    Float2 EventSystem::ControllerLeftStick(uint32_t controllerId, float deadZone)
    {
        Float2 stick(
            ControllerAxis(controllerId, emberCommon::Input::ControllerAxis::LeftX),
            ControllerAxis(controllerId, emberCommon::Input::ControllerAxis::LeftY));
        return ApplyControllerDeadZone(stick, deadZone);
    }
    Float2 EventSystem::ControllerRightStick(uint32_t controllerId, float deadZone)
    {
        Float2 stick(
            ControllerAxis(controllerId, emberCommon::Input::ControllerAxis::RightX),
            ControllerAxis(controllerId, emberCommon::Input::ControllerAxis::RightY));
        return ApplyControllerDeadZone(stick, deadZone);
    }



	// Controller id:
    uint32_t EventSystem::GetMainControllerId()
    {
        if (s_connectedControllers.empty())
            return invalidControllerId;
        return *std::min_element(s_connectedControllers.begin(), s_connectedControllers.end());
    }
    bool EventSystem::ControllerConnected(uint32_t controllerId)
    {
        return s_connectedControllers.find(controllerId) != s_connectedControllers.end();
    }
    std::vector<uint32_t> EventSystem::GetConnectedControllerIds()
    {
        std::vector<uint32_t> controllerIds(s_connectedControllers.begin(), s_connectedControllers.end());
        std::sort(controllerIds.begin(), controllerIds.end());
        return controllerIds;
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
    bool EventSystem::ControllerButtonDownRaw(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        auto controllerIt = s_controllerButtonStates.find(controllerId);
        if (controllerIt == s_controllerButtonStates.end())
            return false;
        auto buttonIt = controllerIt->second.find(button);
        return buttonIt != controllerIt->second.end() && buttonIt->second == ControllerButtonState::down;
    }
    bool EventSystem::ControllerButtonUpRaw(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        auto controllerIt = s_controllerButtonStates.find(controllerId);
        if (controllerIt == s_controllerButtonStates.end())
            return false;
        auto buttonIt = controllerIt->second.find(button);
        return buttonIt != controllerIt->second.end() && buttonIt->second == ControllerButtonState::up;
    }
    bool EventSystem::ControllerButtonHeldRaw(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        auto controllerIt = s_controllerButtonStates.find(controllerId);
        if (controllerIt == s_controllerButtonStates.end())
            return false;
        auto buttonIt = controllerIt->second.find(button);
        return buttonIt != controllerIt->second.end() && buttonIt->second == ControllerButtonState::held;
    }
    bool EventSystem::ControllerButtonDownOrHeldRaw(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        auto controllerIt = s_controllerButtonStates.find(controllerId);
        if (controllerIt == s_controllerButtonStates.end())
            return false;
        auto buttonIt = controllerIt->second.find(button);
        return buttonIt != controllerIt->second.end() && (buttonIt->second == ControllerButtonState::down || buttonIt->second == ControllerButtonState::held);
    }
    float EventSystem::ControllerAxisRaw(uint32_t controllerId, emberCommon::Input::ControllerAxis axis)
    {
        auto controllerIt = s_controllerAxisStates.find(controllerId);
        if (controllerIt == s_controllerAxisStates.end())
            return 0.0f;
        auto axisIt = controllerIt->second.find(axis);
        return axisIt == controllerIt->second.end() ? 0.0f : axisIt->second;
    }
    Float2 EventSystem::ControllerLeftStickRaw(uint32_t controllerId, float deadZone)
    {
        Float2 stick(
            ControllerAxisRaw(controllerId, emberCommon::Input::ControllerAxis::LeftX),
            ControllerAxisRaw(controllerId, emberCommon::Input::ControllerAxis::LeftY));
        return ApplyControllerDeadZone(stick, deadZone);
    }
    Float2 EventSystem::ControllerRightStickRaw(uint32_t controllerId, float deadZone)
    {
        Float2 stick(
            ControllerAxisRaw(controllerId, emberCommon::Input::ControllerAxis::RightX),
            ControllerAxisRaw(controllerId, emberCommon::Input::ControllerAxis::RightY));
        return ApplyControllerDeadZone(stick, deadZone);
    }



	// Event consumption:
    void EventSystem::ConsumeKey(emberCommon::Input::Key key)
    {
        if (key == emberCommon::Input::Key::Unknown || s_currentConsumer == Consumer::none || KeyConsumed(key))
            return;
        s_consumedKeys[key] = s_currentConsumer;
    }
    void EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton button)
    {
        if (button == emberCommon::Input::MouseButton::None || s_currentConsumer == Consumer::none || MouseButtonConsumed(button))
            return;
        s_consumedMouseButtons[button] = s_currentConsumer;
    }
    void EventSystem::ConsumeMouseScroll()
    {
        if (s_currentConsumer == Consumer::none || MouseScrollConsumed())
            return;
        s_mouseScrollConsumer = s_currentConsumer;
    }
    void EventSystem::ConsumeControllerButton(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        if (button == emberCommon::Input::ControllerButton::None
			|| !ControllerConnected(controllerId)
			|| s_currentConsumer == Consumer::none
			|| ControllerButtonConsumed(controllerId, button))
            return;
        s_consumedControllerButtons[controllerId][button] = s_currentConsumer;
    }
    void EventSystem::ConsumeControllerAxis(uint32_t controllerId, emberCommon::Input::ControllerAxis axis)
    {
        if (axis == emberCommon::Input::ControllerAxis::None
			|| !ControllerConnected(controllerId)
			|| s_currentConsumer == Consumer::none
			|| ControllerAxisConsumed(controllerId, axis))
            return;
        s_consumedControllerAxes[controllerId][axis] = s_currentConsumer;
    }
    bool EventSystem::KeyConsumed(emberCommon::Input::Key key)
    {
        return GetKeyConsumer(key) != Consumer::none;
    }
    bool EventSystem::MouseButtonConsumed(emberCommon::Input::MouseButton button)
    {
        return GetMouseButtonConsumer(button) != Consumer::none;
    }
    bool EventSystem::MouseScrollConsumed()
    {
        return s_mouseScrollConsumer != Consumer::none;
    }
    bool EventSystem::ControllerButtonConsumed(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        return GetControllerButtonConsumer(controllerId, button) != Consumer::none;
    }
    bool EventSystem::ControllerAxisConsumed(uint32_t controllerId, emberCommon::Input::ControllerAxis axis)
    {
        return GetControllerAxisConsumer(controllerId, axis) != Consumer::none;
    }
    EventSystem::Consumer EventSystem::GetKeyConsumer(emberCommon::Input::Key key)
    {
        auto it = s_consumedKeys.find(key);
        return it == s_consumedKeys.end() ? Consumer::none : it->second;
    }
    EventSystem::Consumer EventSystem::GetMouseButtonConsumer(emberCommon::Input::MouseButton button)
    {
        auto it = s_consumedMouseButtons.find(button);
        return it == s_consumedMouseButtons.end() ? Consumer::none : it->second;
    }
    EventSystem::Consumer EventSystem::GetMouseScrollConsumer()
    {
        return s_mouseScrollConsumer;
    }
    EventSystem::Consumer EventSystem::GetControllerButtonConsumer(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        auto controllerIt = s_consumedControllerButtons.find(controllerId);
        if (controllerIt == s_consumedControllerButtons.end())
            return Consumer::none;
        auto buttonIt = controllerIt->second.find(button);
        return buttonIt == controllerIt->second.end() ? Consumer::none : buttonIt->second;
    }
    EventSystem::Consumer EventSystem::GetControllerAxisConsumer(uint32_t controllerId, emberCommon::Input::ControllerAxis axis)
    {
        auto controllerIt = s_consumedControllerAxes.find(controllerId);
        if (controllerIt == s_consumedControllerAxes.end())
            return Consumer::none;
        auto axisIt = controllerIt->second.find(axis);
        return axisIt == controllerIt->second.end() ? Consumer::none : axisIt->second;
    }



	// Event locking:
    bool EventSystem::TryLockMouseButton(emberCommon::Input::MouseButton button)
    {
        // No button or no consumer:
        if (button == emberCommon::Input::MouseButton::None || s_currentConsumer == Consumer::none)
            return false;

        // Check if button already locked:
        auto it = s_lockedMouseButtons.find(button);
        if (it != s_lockedMouseButtons.end())
            return it->second == s_currentConsumer;

        // Lock button for current consumer and cancel any pending auto-unlock.
        s_lockedMouseButtons[button] = s_currentConsumer;
        s_mouseButtonLocksPendingUnlock.erase(button);
        return true;
    }
    bool EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton button)
    {
        // Find lock, if not found or owned by other consumer abort:
        auto it = s_lockedMouseButtons.find(button);
        if (it == s_lockedMouseButtons.end() || it->second != s_currentConsumer)
            return false;

        // Unlock button:
        s_lockedMouseButtons.erase(it);
        s_mouseButtonLocksPendingUnlock.erase(button);
        return true;
    }
    bool EventSystem::MouseButtonLocked(emberCommon::Input::MouseButton button)
    {
        return s_lockedMouseButtons.find(button) != s_lockedMouseButtons.end();
    }
    bool EventSystem::MouseButtonLockedBy(emberCommon::Input::MouseButton button, Consumer consumer)
    {
        auto it = s_lockedMouseButtons.find(button);
        return consumer != Consumer::none && it != s_lockedMouseButtons.end() && it->second == consumer;
    }
    EventSystem::Consumer EventSystem::GetMouseButtonLockConsumer(emberCommon::Input::MouseButton button)
    {
        auto it = s_lockedMouseButtons.find(button);
        return it == s_lockedMouseButtons.end() ? Consumer::none : it->second;
    }
    bool EventSystem::TryLockKeyboard()
    {
        // Invalid consumer:
        if (s_currentConsumer == Consumer::none)
            return false;

        // Check if already locked:
        if (s_keyboardLockConsumer != Consumer::none)
            return s_keyboardLockConsumer == s_currentConsumer;

        // Lock keyboard:
        s_keyboardLockConsumer = s_currentConsumer;
        return true;
    }
    bool EventSystem::UnlockKeyboard()
    {
        // Check if locked by current consumer:
        if (s_currentConsumer == Consumer::none || s_keyboardLockConsumer != s_currentConsumer)
            return false;

        // Unlock keyboard:
        s_keyboardLockConsumer = Consumer::none;
        return true;
    }
    bool EventSystem::KeyboardLocked()
    {
        return s_keyboardLockConsumer != Consumer::none;
    }
    bool EventSystem::KeyboardLockedBy(Consumer consumer)
    {
        return consumer != Consumer::none && s_keyboardLockConsumer == consumer;
    }
    EventSystem::Consumer EventSystem::GetKeyboardLockConsumer()
    {
        return s_keyboardLockConsumer;
    }



	// Private methods:
    void EventSystem::SetCurrentConsumer(Consumer consumer)
    {
        s_currentConsumer = consumer;
    }
    EventSystem::Consumer EventSystem::GetCurrentConsumer()
    {
        return s_currentConsumer;
    }
    void EventSystem::ClearInputState()
    {
        s_keyStates.clear();
        s_mouseButtonStates.clear();
        s_controllerButtonStates.clear();
        s_controllerAxisStates.clear();
        s_lockedMouseButtons.clear();
        s_mouseButtonLocksPendingUnlock.clear();
        s_keyboardLockConsumer = Consumer::none;
        ClearConsumedEvents();
        s_mouseScrollX = 0;
        s_mouseScrollY = 0;
    }
    void EventSystem::ClearEvents()
    {
        UnlockPendingMouseButtons();
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

        // Reset released controller buttons and transition pressed controller buttons to held:
        for (auto& [controllerId, buttonStates] : s_controllerButtonStates)
        {
            for (auto& [button, state] : buttonStates)
            {
                if (state == ControllerButtonState::down)
                    state = ControllerButtonState::held;
                else if (state == ControllerButtonState::up)
                    state = ControllerButtonState::none;
            }
        }

        // Reset mouse scroll:
        s_mouseScrollX = 0;
        s_mouseScrollY = 0;
    }
    void EventSystem::UnlockPendingMouseButtons()
    {
        for (emberCommon::Input::MouseButton button : s_mouseButtonLocksPendingUnlock)
            s_lockedMouseButtons.erase(button);
        s_mouseButtonLocksPendingUnlock.clear();
    }
    void EventSystem::ClearConsumedEvents()
    {
        s_consumedKeys.clear();
        s_consumedMouseButtons.clear();
        s_consumedControllerButtons.clear();
        s_consumedControllerAxes.clear();
        s_mouseScrollConsumer = Consumer::none;
    }
    bool EventSystem::KeyBlocked(emberCommon::Input::Key key)
    {
        Consumer keyConsumer = GetKeyConsumer(key);
        bool keyConsumedByOther = keyConsumer != Consumer::none && keyConsumer != s_currentConsumer;
        bool keyboardLockedByOther = s_keyboardLockConsumer != Consumer::none && s_keyboardLockConsumer != s_currentConsumer;
        return keyConsumedByOther || keyboardLockedByOther;
    }
    bool EventSystem::MouseButtonBlocked(emberCommon::Input::MouseButton button)
    {
        Consumer buttonConsumer = GetMouseButtonConsumer(button);
        Consumer lockConsumer = GetMouseButtonLockConsumer(button);
        bool buttonConsumedByOther = buttonConsumer != Consumer::none && buttonConsumer != s_currentConsumer;
        bool buttonLockedByOther = lockConsumer != Consumer::none && lockConsumer != s_currentConsumer;
        return buttonConsumedByOther || buttonLockedByOther;
    }
    bool EventSystem::MouseScrollBlocked()
    {
        return s_mouseScrollConsumer != Consumer::none && s_mouseScrollConsumer != s_currentConsumer;
    }
    bool EventSystem::ControllerButtonBlocked(uint32_t controllerId, emberCommon::Input::ControllerButton button)
    {
        Consumer buttonConsumer = GetControllerButtonConsumer(controllerId, button);
        return buttonConsumer != Consumer::none && buttonConsumer != s_currentConsumer;
    }
    bool EventSystem::ControllerAxisBlocked(uint32_t controllerId, emberCommon::Input::ControllerAxis axis)
    {
        Consumer axisConsumer = GetControllerAxisConsumer(controllerId, axis);
        return axisConsumer != Consumer::none && axisConsumer != s_currentConsumer;
    }
    bool EventSystem::ProcessGuiEventFilter(const emberCommon::Event& event)
    {
        switch (event.type)
        {
            case emberCommon::EventType::MouseMoved:
                return !AnyMouseButtonLockedBy(Consumer::gui) && !event.guiWantsMouse;
            case emberCommon::EventType::MouseButtonDown:
                if (event.guiWantsMouse)
                {
                    if (event.mouseButton != emberCommon::Input::MouseButton::None)
                    {
                        s_lockedMouseButtons[event.mouseButton] = Consumer::gui;
                        s_mouseButtonLocksPendingUnlock.erase(event.mouseButton);
                    }
                    return false;
                }
                return true;
            case emberCommon::EventType::MouseButtonUp:
                if (MouseButtonLockedBy(event.mouseButton, Consumer::gui))
                {
                    s_lockedMouseButtons.erase(event.mouseButton);
                    s_mouseButtonLocksPendingUnlock.erase(event.mouseButton);
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
    bool EventSystem::AnyMouseButtonLockedBy(Consumer consumer)
    {
        for (const auto& lock : s_lockedMouseButtons)
            if (lock.second == consumer)
                return true;
        return false;
    }
    Float2 EventSystem::ApplyControllerDeadZone(const Float2& stick, float deadZone)
    {
        if (deadZone <= 0.0f)
            return stick;
        if (deadZone >= 1.0f)
            return Float2::zero;

        float stickLength = stick.Length();
        if (stickLength <= deadZone)
            return Float2::zero;
        return stick * ((stickLength - deadZone) / (stickLength * (1.0f - deadZone)));
    }



    // EventConsumerScope:
    EventConsumerScope::EventConsumerScope(EventSystem::Consumer consumer) :
        m_previousConsumer(EventSystem::GetCurrentConsumer())
    {
        EventSystem::SetCurrentConsumer(consumer);
    }
    EventConsumerScope::~EventConsumerScope()
    {
        EventSystem::SetCurrentConsumer(m_previousConsumer);
    }
}