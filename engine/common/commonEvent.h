#pragma once
#include "commonInput.h"
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>



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

    inline constexpr std::string_view EventTypeToString(EventType type)
    {
        switch (type)
        {
            case EventType::None: return "None";
            case EventType::Quit: return "Quit";
            case EventType::WindowClose: return "WindowClose";
            case EventType::WindowResized: return "WindowResized";
            case EventType::WindowMinimized: return "WindowMinimized";
            case EventType::WindowRestored: return "WindowRestored";
            case EventType::WindowFocusGained: return "WindowFocusGained";
            case EventType::WindowFocusLost: return "WindowFocusLost";
            case EventType::KeyDown: return "KeyDown";
            case EventType::KeyUp: return "KeyUp";
            case EventType::TextInput: return "TextInput";
            case EventType::MouseMoved: return "MouseMoved";
            case EventType::MouseButtonDown: return "MouseButtonDown";
            case EventType::MouseButtonUp: return "MouseButtonUp";
            case EventType::MouseWheel: return "MouseWheel";
            case EventType::ControllerConnected: return "ControllerConnected";
            case EventType::ControllerDisconnected: return "ControllerDisconnected";
            case EventType::ControllerButtonDown: return "ControllerButtonDown";
            case EventType::ControllerButtonUp: return "ControllerButtonUp";
            case EventType::ControllerAxisMotion: return "ControllerAxisMotion";
            default: return "Unknown";
        }
    }

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

        inline std::string ToString() const
        {
            std::ostringstream ss;
            ss << "type: " << EventTypeToString(type);
            ss << ", key: " << Input::KeyToString(key);
            ss << ", mouseButton: " << Input::MouseButtonToString(mouseButton);
            ss << ", mousePos: (" << mousePosX << ", " << mousePosY << ")";
            ss << ", mouseWheel: (" << mouseWheelX << ", " << mouseWheelY << ")";
            ss << ", controllerId: " << controllerId;
            ss << ", controllerButton: " << Input::ControllerButtonToString(controllerButton);
            ss << ", axis: " << static_cast<uint32_t>(axis);
            ss << ", axisValue: " << axisValue;
            ss << ", text: " << text;
            ss << ", windowID: " << windowID;
            ss << ", resize: (" << resizeWidth << ", " << resizeHeight << ")";
            return ss.str();
        }
    };
}