#ifndef __INCLUDE_GUARD_sdlEventTranslation_h__
#define __INCLUDE_GUARD_sdlEventTranslation_h__
#include "input.h"
#include <SDL3/SDL.h>



namespace sdlWindowBackend
{
    emberEngine::Input::MouseButton TranslateMouseButton(uint8_t sdlMouseButton)
    {
        switch (sdlMouseButton)
        {
            case SDL_BUTTON_LEFT:    return emberEngine::Input::MouseButton::Left;
            case SDL_BUTTON_RIGHT:   return emberEngine::Input::MouseButton::Right;
            case SDL_BUTTON_MIDDLE:  return emberEngine::Input::MouseButton::Middle;
            case SDL_BUTTON_X1:      return emberEngine::Input::MouseButton::X1;
            case SDL_BUTTON_X2:      return emberEngine::Input::MouseButton::X2;
			default:                 return emberEngine::Input::MouseButton::None;
        }
    }
    
    emberEngine::Input::ControllerButton TranslateControllerButton(uint8_t sdlControllerButton)
    {
        switch (sdlControllerButton)
        {
            case SDL_GAMEPAD_BUTTON_SOUTH:          return emberEngine::Input::ControllerButton::South;
            case SDL_GAMEPAD_BUTTON_EAST:           return emberEngine::Input::ControllerButton::East;
            case SDL_GAMEPAD_BUTTON_WEST:           return emberEngine::Input::ControllerButton::West;
            case SDL_GAMEPAD_BUTTON_NORTH:          return emberEngine::Input::ControllerButton::North;
            case SDL_GAMEPAD_BUTTON_BACK:           return emberEngine::Input::ControllerButton::Back;
            case SDL_GAMEPAD_BUTTON_GUIDE:          return emberEngine::Input::ControllerButton::Guide;
            case SDL_GAMEPAD_BUTTON_START:          return emberEngine::Input::ControllerButton::Start;
            case SDL_GAMEPAD_BUTTON_LEFT_STICK:     return emberEngine::Input::ControllerButton::LeftStick;
            case SDL_GAMEPAD_BUTTON_RIGHT_STICK:    return emberEngine::Input::ControllerButton::RightStick;
            case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:  return emberEngine::Input::ControllerButton::LeftShoulder;
            case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: return emberEngine::Input::ControllerButton::RightShoulder;
            case SDL_GAMEPAD_BUTTON_DPAD_UP:        return emberEngine::Input::ControllerButton::DpadUp;
            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:      return emberEngine::Input::ControllerButton::DpadDown;
            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:      return emberEngine::Input::ControllerButton::DpadLeft;
			case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:     return emberEngine::Input::ControllerButton::DpadRight;
            default:                                return emberEngine::Input::ControllerButton::None;
        }
	}

    emberEngine::Input::Key TranslateKey(SDL_Keycode sdlKey)
    {
        switch (sdlKey)
        {
            // Unknown:
            case SDLK_UNKNOWN:      return emberEngine::Input::Key::Unknown;
                
            // Letters:
            case SDLK_A:            return emberEngine::Input::Key::A;
            case SDLK_B:            return emberEngine::Input::Key::B;
            case SDLK_C:            return emberEngine::Input::Key::C;
            case SDLK_D:            return emberEngine::Input::Key::D;
            case SDLK_E:            return emberEngine::Input::Key::E;
            case SDLK_F:            return emberEngine::Input::Key::F;
            case SDLK_G:            return emberEngine::Input::Key::G;
            case SDLK_H:            return emberEngine::Input::Key::H;
            case SDLK_I:            return emberEngine::Input::Key::I;
            case SDLK_J:            return emberEngine::Input::Key::J;
            case SDLK_K:            return emberEngine::Input::Key::K;
            case SDLK_L:            return emberEngine::Input::Key::L;
            case SDLK_M:            return emberEngine::Input::Key::M;
            case SDLK_N:            return emberEngine::Input::Key::N;
            case SDLK_O:            return emberEngine::Input::Key::O;
            case SDLK_P:            return emberEngine::Input::Key::P;
            case SDLK_Q:            return emberEngine::Input::Key::Q;
            case SDLK_R:            return emberEngine::Input::Key::R;
            case SDLK_S:            return emberEngine::Input::Key::S;
            case SDLK_T:            return emberEngine::Input::Key::T;
            case SDLK_U:            return emberEngine::Input::Key::U;
            case SDLK_V:            return emberEngine::Input::Key::V;
            case SDLK_W:            return emberEngine::Input::Key::W;
            case SDLK_X:            return emberEngine::Input::Key::X;
            case SDLK_Y:            return emberEngine::Input::Key::Y;
            case SDLK_Z:            return emberEngine::Input::Key::Z;

            // Main digits:
            case SDLK_0:            return emberEngine::Input::Key::Num0;
            case SDLK_1:            return emberEngine::Input::Key::Num1;
            case SDLK_2:            return emberEngine::Input::Key::Num2;
            case SDLK_3:            return emberEngine::Input::Key::Num3;
            case SDLK_4:            return emberEngine::Input::Key::Num4;
            case SDLK_5:            return emberEngine::Input::Key::Num5;
            case SDLK_6:            return emberEngine::Input::Key::Num6;
            case SDLK_7:            return emberEngine::Input::Key::Num7;
            case SDLK_8:            return emberEngine::Input::Key::Num8;
            case SDLK_9:            return emberEngine::Input::Key::Num9;

            // Numpad digits:
            case SDLK_KP_0:         return emberEngine::Input::Key::Numpad0;
            case SDLK_KP_1:         return emberEngine::Input::Key::Numpad1;
            case SDLK_KP_2:         return emberEngine::Input::Key::Numpad2;
            case SDLK_KP_3:         return emberEngine::Input::Key::Numpad3;
            case SDLK_KP_4:         return emberEngine::Input::Key::Numpad4;
            case SDLK_KP_5:         return emberEngine::Input::Key::Numpad5;
            case SDLK_KP_6:         return emberEngine::Input::Key::Numpad6;
            case SDLK_KP_7:         return emberEngine::Input::Key::Numpad7;
            case SDLK_KP_8:         return emberEngine::Input::Key::Numpad8;
            case SDLK_KP_9:         return emberEngine::Input::Key::Numpad9;

            // Symbols:
			case SDLK_PLUS:         return emberEngine::Input::Key::Plus;
			case SDLK_MINUS:        return emberEngine::Input::Key::Minus;
			case SDLK_EQUALS:       return emberEngine::Input::Key::Equal;
			case SDLK_SLASH:        return emberEngine::Input::Key::Slash;
			case SDLK_BACKSLASH:    return emberEngine::Input::Key::Backslash;
			case SDLK_COMMA:        return emberEngine::Input::Key::Comma;
			case SDLK_PERIOD:       return emberEngine::Input::Key::Period;
			case SDLK_SEMICOLON:    return emberEngine::Input::Key::Semicolon;
			case SDLK_APOSTROPHE:   return emberEngine::Input::Key::Apostrophe;

            // Controls:
            case SDLK_ESCAPE:       return emberEngine::Input::Key::Escape;
            case SDLK_SPACE:        return emberEngine::Input::Key::Space;
            case SDLK_TAB:          return emberEngine::Input::Key::Tab;
            case SDLK_RETURN:       return emberEngine::Input::Key::Enter;
            case SDLK_BACKSPACE:    return emberEngine::Input::Key::Backspace;

            // Arrows:
            case SDLK_LEFT:         return emberEngine::Input::Key::Left;
            case SDLK_RIGHT:        return emberEngine::Input::Key::Right;
            case SDLK_UP:           return emberEngine::Input::Key::Up;
            case SDLK_DOWN:         return emberEngine::Input::Key::Down;


            // Function keys:
            case SDLK_F1:           return emberEngine::Input::Key::F1;
            case SDLK_F2:           return emberEngine::Input::Key::F2;
            case SDLK_F3:           return emberEngine::Input::Key::F3;
            case SDLK_F4:           return emberEngine::Input::Key::F4;
            case SDLK_F5:           return emberEngine::Input::Key::F5;
            case SDLK_F6:           return emberEngine::Input::Key::F6;
            case SDLK_F7:           return emberEngine::Input::Key::F7;
            case SDLK_F8:           return emberEngine::Input::Key::F8;
            case SDLK_F9:           return emberEngine::Input::Key::F9;
            case SDLK_F10:          return emberEngine::Input::Key::F10;
            case SDLK_F11:          return emberEngine::Input::Key::F11;
            case SDLK_F12:          return emberEngine::Input::Key::F12;

            // Modifiers:
            case SDLK_LSHIFT:       return emberEngine::Input::Key::ShiftLeft;
            case SDLK_RSHIFT:       return emberEngine::Input::Key::ShiftRight;
            case SDLK_LCTRL:        return emberEngine::Input::Key::CtrlLeft;
            case SDLK_RCTRL:        return emberEngine::Input::Key::CtrlRight;
            case SDLK_LALT:         return emberEngine::Input::Key::AltLeft;
            case SDLK_RALT:         return emberEngine::Input::Key::AltRight;
            case SDLK_LGUI:         return emberEngine::Input::Key::MetaLeft;
            case SDLK_RGUI:         return emberEngine::Input::Key::MetaRight;

            default: return emberEngine::Input::Key::Unknown;
        }
    }
}



#endif // __INCLUDE_GUARD_sdlEventTranslation_h__