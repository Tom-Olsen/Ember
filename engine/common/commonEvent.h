#pragma once
#include "commonInput.h"
#include <string>



namespace emberCommon
{
    enum class EventType
    {
        None,

        // System / Window:
        Quit,
        WindowClose,
        WindowResized,
        WindowMinimized,
        WindowRestored,
        WindowFocusGained,
        WindowFocusLost,

        // Keyboard:
        KeyDown,
        KeyUp,
        TextInput,

        // Mouse:
        MouseMoved,
        MouseButtonDown,
        MouseButtonUp,
        MouseWheel,

        // Controller:
        ControllerConnected,
        ControllerDisconnected,
        ControllerButtonDown,
        ControllerButtonUp,
        ControllerAxisMotion,
    };

    struct Event
    {
        // Event type:
        EventType type{ EventType::None };

        // Key:
        Input::Key key{ Input::Key::Unknown };

        // Mouse:
		Input::MouseButton mouseButton{ Input::MouseButton::None };
        int mousePosX{ 0 }, mousePosY{ 0 };
		float mouseWheelX{ 0.0f }, mouseWheelY{ 0.0f };

        // Controller:
		uint32_t controllerId{ 0 };
		Input::ControllerButton controllerButton{ Input::ControllerButton::None };
        uint8_t axis{ 0 };
		int16_t axisValue{ 0 };

        // Text input:
        std::string text;

        // Window resize:
		uint32_t windowID{ 0 };
		uint32_t resizeWidth{ 0 }, resizeHeight{ 0 };
    };
}