#pragma once
#include "commonInput.h"
#include <SDL3/SDL.h>



namespace sdlWindowBackend
{
    emberCommon::Input::MouseButton TranslateMouseButton(uint8_t sdlMouseButton)
    {
        switch (sdlMouseButton)
        {
            case SDL_BUTTON_LEFT:    return emberCommon::Input::MouseButton::Left;
            case SDL_BUTTON_RIGHT:   return emberCommon::Input::MouseButton::Right;
            case SDL_BUTTON_MIDDLE:  return emberCommon::Input::MouseButton::Middle;
            case SDL_BUTTON_X1:      return emberCommon::Input::MouseButton::X1;
            case SDL_BUTTON_X2:      return emberCommon::Input::MouseButton::X2;
			default:                 return emberCommon::Input::MouseButton::None;
        }
    }
    
    emberCommon::Input::ControllerButton TranslateControllerButton(uint8_t sdlControllerButton)
    {
        switch (sdlControllerButton)
        {
            case SDL_GAMEPAD_BUTTON_SOUTH:          return emberCommon::Input::ControllerButton::South;
            case SDL_GAMEPAD_BUTTON_EAST:           return emberCommon::Input::ControllerButton::East;
            case SDL_GAMEPAD_BUTTON_WEST:           return emberCommon::Input::ControllerButton::West;
            case SDL_GAMEPAD_BUTTON_NORTH:          return emberCommon::Input::ControllerButton::North;
            case SDL_GAMEPAD_BUTTON_BACK:           return emberCommon::Input::ControllerButton::Back;
            case SDL_GAMEPAD_BUTTON_GUIDE:          return emberCommon::Input::ControllerButton::Guide;
            case SDL_GAMEPAD_BUTTON_START:          return emberCommon::Input::ControllerButton::Start;
            case SDL_GAMEPAD_BUTTON_LEFT_STICK:     return emberCommon::Input::ControllerButton::LeftStick;
            case SDL_GAMEPAD_BUTTON_RIGHT_STICK:    return emberCommon::Input::ControllerButton::RightStick;
            case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:  return emberCommon::Input::ControllerButton::LeftShoulder;
            case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: return emberCommon::Input::ControllerButton::RightShoulder;
            case SDL_GAMEPAD_BUTTON_DPAD_UP:        return emberCommon::Input::ControllerButton::DpadUp;
            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:      return emberCommon::Input::ControllerButton::DpadDown;
            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:      return emberCommon::Input::ControllerButton::DpadLeft;
			case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:     return emberCommon::Input::ControllerButton::DpadRight;
            default:                                return emberCommon::Input::ControllerButton::None;
        }
	}

    emberCommon::Input::Key TranslateKey(SDL_Keycode sdlKey)
    {
        switch (sdlKey)
        {
            // Unknown:
            case SDLK_UNKNOWN:      return emberCommon::Input::Key::Unknown;
                
            // Letters:
            case SDLK_A:            return emberCommon::Input::Key::A;
            case SDLK_B:            return emberCommon::Input::Key::B;
            case SDLK_C:            return emberCommon::Input::Key::C;
            case SDLK_D:            return emberCommon::Input::Key::D;
            case SDLK_E:            return emberCommon::Input::Key::E;
            case SDLK_F:            return emberCommon::Input::Key::F;
            case SDLK_G:            return emberCommon::Input::Key::G;
            case SDLK_H:            return emberCommon::Input::Key::H;
            case SDLK_I:            return emberCommon::Input::Key::I;
            case SDLK_J:            return emberCommon::Input::Key::J;
            case SDLK_K:            return emberCommon::Input::Key::K;
            case SDLK_L:            return emberCommon::Input::Key::L;
            case SDLK_M:            return emberCommon::Input::Key::M;
            case SDLK_N:            return emberCommon::Input::Key::N;
            case SDLK_O:            return emberCommon::Input::Key::O;
            case SDLK_P:            return emberCommon::Input::Key::P;
            case SDLK_Q:            return emberCommon::Input::Key::Q;
            case SDLK_R:            return emberCommon::Input::Key::R;
            case SDLK_S:            return emberCommon::Input::Key::S;
            case SDLK_T:            return emberCommon::Input::Key::T;
            case SDLK_U:            return emberCommon::Input::Key::U;
            case SDLK_V:            return emberCommon::Input::Key::V;
            case SDLK_W:            return emberCommon::Input::Key::W;
            case SDLK_X:            return emberCommon::Input::Key::X;
            case SDLK_Y:            return emberCommon::Input::Key::Y;
            case SDLK_Z:            return emberCommon::Input::Key::Z;

            // Main digits:
            case SDLK_0:            return emberCommon::Input::Key::Num0;
            case SDLK_1:            return emberCommon::Input::Key::Num1;
            case SDLK_2:            return emberCommon::Input::Key::Num2;
            case SDLK_3:            return emberCommon::Input::Key::Num3;
            case SDLK_4:            return emberCommon::Input::Key::Num4;
            case SDLK_5:            return emberCommon::Input::Key::Num5;
            case SDLK_6:            return emberCommon::Input::Key::Num6;
            case SDLK_7:            return emberCommon::Input::Key::Num7;
            case SDLK_8:            return emberCommon::Input::Key::Num8;
            case SDLK_9:            return emberCommon::Input::Key::Num9;

            // Numpad digits:
            case SDLK_KP_0:         return emberCommon::Input::Key::Numpad0;
            case SDLK_KP_1:         return emberCommon::Input::Key::Numpad1;
            case SDLK_KP_2:         return emberCommon::Input::Key::Numpad2;
            case SDLK_KP_3:         return emberCommon::Input::Key::Numpad3;
            case SDLK_KP_4:         return emberCommon::Input::Key::Numpad4;
            case SDLK_KP_5:         return emberCommon::Input::Key::Numpad5;
            case SDLK_KP_6:         return emberCommon::Input::Key::Numpad6;
            case SDLK_KP_7:         return emberCommon::Input::Key::Numpad7;
            case SDLK_KP_8:         return emberCommon::Input::Key::Numpad8;
            case SDLK_KP_9:         return emberCommon::Input::Key::Numpad9;

            // Symbols:
			case SDLK_PLUS:         return emberCommon::Input::Key::Plus;
			case SDLK_MINUS:        return emberCommon::Input::Key::Minus;
			case SDLK_EQUALS:       return emberCommon::Input::Key::Equal;
			case SDLK_SLASH:        return emberCommon::Input::Key::Slash;
			case SDLK_BACKSLASH:    return emberCommon::Input::Key::Backslash;
			case SDLK_COMMA:        return emberCommon::Input::Key::Comma;
			case SDLK_PERIOD:       return emberCommon::Input::Key::Period;
			case SDLK_SEMICOLON:    return emberCommon::Input::Key::Semicolon;
			case SDLK_APOSTROPHE:   return emberCommon::Input::Key::Apostrophe;

            // Controls:
            case SDLK_ESCAPE:       return emberCommon::Input::Key::Escape;
            case SDLK_SPACE:        return emberCommon::Input::Key::Space;
            case SDLK_TAB:          return emberCommon::Input::Key::Tab;
            case SDLK_RETURN:       return emberCommon::Input::Key::Enter;
            case SDLK_BACKSPACE:    return emberCommon::Input::Key::Backspace;

            // Arrows:
            case SDLK_LEFT:         return emberCommon::Input::Key::Left;
            case SDLK_RIGHT:        return emberCommon::Input::Key::Right;
            case SDLK_UP:           return emberCommon::Input::Key::Up;
            case SDLK_DOWN:         return emberCommon::Input::Key::Down;


            // Function keys:
            case SDLK_F1:           return emberCommon::Input::Key::F1;
            case SDLK_F2:           return emberCommon::Input::Key::F2;
            case SDLK_F3:           return emberCommon::Input::Key::F3;
            case SDLK_F4:           return emberCommon::Input::Key::F4;
            case SDLK_F5:           return emberCommon::Input::Key::F5;
            case SDLK_F6:           return emberCommon::Input::Key::F6;
            case SDLK_F7:           return emberCommon::Input::Key::F7;
            case SDLK_F8:           return emberCommon::Input::Key::F8;
            case SDLK_F9:           return emberCommon::Input::Key::F9;
            case SDLK_F10:          return emberCommon::Input::Key::F10;
            case SDLK_F11:          return emberCommon::Input::Key::F11;
            case SDLK_F12:          return emberCommon::Input::Key::F12;

            // Modifiers:
            case SDLK_LSHIFT:       return emberCommon::Input::Key::ShiftLeft;
            case SDLK_RSHIFT:       return emberCommon::Input::Key::ShiftRight;
            case SDLK_LCTRL:        return emberCommon::Input::Key::CtrlLeft;
            case SDLK_RCTRL:        return emberCommon::Input::Key::CtrlRight;
            case SDLK_LALT:         return emberCommon::Input::Key::AltLeft;
            case SDLK_RALT:         return emberCommon::Input::Key::AltRight;
            case SDLK_LGUI:         return emberCommon::Input::Key::MetaLeft;
            case SDLK_RGUI:         return emberCommon::Input::Key::MetaRight;

            // Editing / navigation:
            case SDLK_INSERT:       return emberCommon::Input::Key::Insert;
            case SDLK_DELETE:       return emberCommon::Input::Key::Delete;
            case SDLK_HOME:         return emberCommon::Input::Key::Home;
            case SDLK_END:          return emberCommon::Input::Key::End;
            case SDLK_PAGEUP:       return emberCommon::Input::Key::PageUp;
            case SDLK_PAGEDOWN:     return emberCommon::Input::Key::PageDown;

            // Lock keys:
            case SDLK_CAPSLOCK:     return emberCommon::Input::Key::CapsLock;
            case SDLK_NUMLOCKCLEAR: return emberCommon::Input::Key::NumLock;
            case SDLK_SCROLLLOCK:   return emberCommon::Input::Key::ScrollLock;

            // Misc:
            case SDLK_PRINTSCREEN:  return emberCommon::Input::Key::PrintScreen;
            case SDLK_PAUSE:        return emberCommon::Input::Key::Pause;

            default: return emberCommon::Input::Key::Unknown;
        }
    }
}