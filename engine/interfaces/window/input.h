#ifndef __INCLUDE_GUARD_input_h__
#define __INCLUDE_GUARD_input_h__



// The input system is supposed to be accessable in the ECS.
// So it should reside in the emberEngine namespace.
namespace emberEngine
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
            MetaLeft, MetaRight // windows key.
        };
    };
}



#endif // __INCLUDE_GUARD_input_h__