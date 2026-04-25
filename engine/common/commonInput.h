#pragma once
#include <string_view>



namespace emberCommon
{
    struct Input
    {
        enum class MouseButton
        {
            None,
            Left,
            Right,
            Middle,
            X1,
            X2
        };

        enum class ControllerButton
        {
            None,
            South,
            East,
            West,
            North,
            Back,
            Guide,
            Start,
            LeftStick,
            RightStick,
            LeftShoulder,
            RightShoulder,
            DpadUp,
            DpadDown,
            DpadLeft,
            DpadRight
		};

        enum class Key
        {
            Unknown,

            // Letters:
            A, B, C, D, E, F, G,
            H, I, J, K, L, M, N,
            O, P, Q, R, S, T, U,
            V, W, X, Y, Z,

            // Main digits:
            Num0, Num1, Num2, Num3, Num4,
            Num5, Num6, Num7, Num8, Num9,

            // Numpad digits:
            Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
            Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
            NumpadAdd, NumpadSubtract, NumpadMultiply, NumpadDivide, NumpadEnter,

            // Symbols:
            Plus, Minus, Equal, Slash, Backslash, Comma, Period, Semicolon, Apostrophe,

            // Controls:
            Escape, Space, Tab, Enter, Backspace,

            // Arrows:
            Left, Right, Up, Down,

            // Function keys:
            F1, F2, F3, F4, F5, F6,
            F7, F8, F9, F10, F11, F12,

            // Modifiers:
            ShiftLeft, ShiftRight,
            CtrlLeft, CtrlRight,
            AltLeft, AltRight,
            MetaLeft, MetaRight, // windows key.

            // Editing/Navigation keys:
            Insert,
            Delete,
            Home,
            End,
            PageUp,
            PageDown,

            // Lock keys:
            CapsLock,
            NumLock,
            ScrollLock,

            // Misc:
            PrintScreen,
            Pause
        };

        inline static constexpr std::string_view MouseButtonToString(MouseButton button)
        {
            switch (button)
            {
                case MouseButton::None: return "None";
                case MouseButton::Left: return "Left";
                case MouseButton::Right: return "Right";
                case MouseButton::Middle: return "Middle";
                case MouseButton::X1: return "X1";
                case MouseButton::X2: return "X2";
                default: return "Unknown";
            }
        }

        inline static constexpr std::string_view ControllerButtonToString(ControllerButton button)
        {
            switch (button)
            {
                case ControllerButton::None: return "None";
                case ControllerButton::South: return "South";
                case ControllerButton::East: return "East";
                case ControllerButton::West: return "West";
                case ControllerButton::North: return "North";
                case ControllerButton::Back: return "Back";
                case ControllerButton::Guide: return "Guide";
                case ControllerButton::Start: return "Start";
                case ControllerButton::LeftStick: return "LeftStick";
                case ControllerButton::RightStick: return "RightStick";
                case ControllerButton::LeftShoulder: return "LeftShoulder";
                case ControllerButton::RightShoulder: return "RightShoulder";
                case ControllerButton::DpadUp: return "DpadUp";
                case ControllerButton::DpadDown: return "DpadDown";
                case ControllerButton::DpadLeft: return "DpadLeft";
                case ControllerButton::DpadRight: return "DpadRight";
                default: return "Unknown";
            }
        }

        inline static constexpr std::string_view KeyToString(Key key)
        {
            switch (key)
            {
                case Key::Unknown: return "Unknown";
                case Key::A: return "A";
                case Key::B: return "B";
                case Key::C: return "C";
                case Key::D: return "D";
                case Key::E: return "E";
                case Key::F: return "F";
                case Key::G: return "G";
                case Key::H: return "H";
                case Key::I: return "I";
                case Key::J: return "J";
                case Key::K: return "K";
                case Key::L: return "L";
                case Key::M: return "M";
                case Key::N: return "N";
                case Key::O: return "O";
                case Key::P: return "P";
                case Key::Q: return "Q";
                case Key::R: return "R";
                case Key::S: return "S";
                case Key::T: return "T";
                case Key::U: return "U";
                case Key::V: return "V";
                case Key::W: return "W";
                case Key::X: return "X";
                case Key::Y: return "Y";
                case Key::Z: return "Z";
                case Key::Num0: return "Num0";
                case Key::Num1: return "Num1";
                case Key::Num2: return "Num2";
                case Key::Num3: return "Num3";
                case Key::Num4: return "Num4";
                case Key::Num5: return "Num5";
                case Key::Num6: return "Num6";
                case Key::Num7: return "Num7";
                case Key::Num8: return "Num8";
                case Key::Num9: return "Num9";
                case Key::Numpad0: return "Numpad0";
                case Key::Numpad1: return "Numpad1";
                case Key::Numpad2: return "Numpad2";
                case Key::Numpad3: return "Numpad3";
                case Key::Numpad4: return "Numpad4";
                case Key::Numpad5: return "Numpad5";
                case Key::Numpad6: return "Numpad6";
                case Key::Numpad7: return "Numpad7";
                case Key::Numpad8: return "Numpad8";
                case Key::Numpad9: return "Numpad9";
                case Key::NumpadAdd: return "NumpadAdd";
                case Key::NumpadSubtract: return "NumpadSubtract";
                case Key::NumpadMultiply: return "NumpadMultiply";
                case Key::NumpadDivide: return "NumpadDivide";
                case Key::NumpadEnter: return "NumpadEnter";
                case Key::Plus: return "Plus";
                case Key::Minus: return "Minus";
                case Key::Equal: return "Equal";
                case Key::Slash: return "Slash";
                case Key::Backslash: return "Backslash";
                case Key::Comma: return "Comma";
                case Key::Period: return "Period";
                case Key::Semicolon: return "Semicolon";
                case Key::Apostrophe: return "Apostrophe";
                case Key::Escape: return "Escape";
                case Key::Space: return "Space";
                case Key::Tab: return "Tab";
                case Key::Enter: return "Enter";
                case Key::Backspace: return "Backspace";
                case Key::Left: return "Left";
                case Key::Right: return "Right";
                case Key::Up: return "Up";
                case Key::Down: return "Down";
                case Key::F1: return "F1";
                case Key::F2: return "F2";
                case Key::F3: return "F3";
                case Key::F4: return "F4";
                case Key::F5: return "F5";
                case Key::F6: return "F6";
                case Key::F7: return "F7";
                case Key::F8: return "F8";
                case Key::F9: return "F9";
                case Key::F10: return "F10";
                case Key::F11: return "F11";
                case Key::F12: return "F12";
                case Key::ShiftLeft: return "ShiftLeft";
                case Key::ShiftRight: return "ShiftRight";
                case Key::CtrlLeft: return "CtrlLeft";
                case Key::CtrlRight: return "CtrlRight";
                case Key::AltLeft: return "AltLeft";
                case Key::AltRight: return "AltRight";
                case Key::MetaLeft: return "MetaLeft";
                case Key::MetaRight: return "MetaRight";
                case Key::Insert: return "Insert";
                case Key::Delete: return "Delete";
                case Key::Home: return "Home";
                case Key::End: return "End";
                case Key::PageUp: return "PageUp";
                case Key::PageDown: return "PageDown";
                case Key::CapsLock: return "CapsLock";
                case Key::NumLock: return "NumLock";
                case Key::ScrollLock: return "ScrollLock";
                case Key::PrintScreen: return "PrintScreen";
                case Key::Pause: return "Pause";
                default: return "Unknown";
            }
        }
    };
}