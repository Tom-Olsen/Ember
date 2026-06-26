#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"
#include "commonInput.h"
#include <string>
#include <unordered_map>
#include <unordered_set>



namespace emberCommon
{
    struct Event;
}



namespace emberCore
{
    class EMBER_CORE_API EventSystem
    {
    public: // Enums:
        enum class KeyState { none, down, up, held };
        enum class MouseState { none, down, up, held };

    private: // Members
        static bool s_isInitialized;
        static std::unordered_map<emberCommon::Input::Key, KeyState> s_keyStates;
        static std::unordered_map<emberCommon::Input::MouseButton, MouseState> s_mouseButtonStates;
        static std::unordered_set<emberCommon::Input::MouseButton> s_guiCapturedMouseButtons;
        static std::unordered_map<emberCommon::Input::Key, std::string> s_consumedKeys;
        static std::unordered_map<emberCommon::Input::MouseButton, std::string> s_consumedMouseButtons;
        static std::string s_emptyConsumer;
        static std::string s_mouseScrollConsumer;
        static bool s_mouseScrollConsumed;
        static float s_mouseX;
        static float s_mouseY;
        static float s_mouseScrollX;
        static float s_mouseScrollY;
		static int s_windowWidth;
		static int s_windowHeight;

    public: // Methods:
        static void Init();
        static void Clear();

        // Event handling:
        static bool ProcessEvents();

        // Event queries:
        static bool AnyKeyDown();
        static bool AnyKeyUp();
        static bool AnyKeyHeld();
        static bool AnyMouseDown();
        static bool AnyMouseUp();
        static bool AnyMouseHeld();
        static bool KeyDown(emberCommon::Input::Key key);
        static bool KeyUp(emberCommon::Input::Key key);
        static bool KeyHeld(emberCommon::Input::Key key);
        static bool KeyDownOrHeld(emberCommon::Input::Key key);
        static bool MouseDown(emberCommon::Input::MouseButton button);
        static bool MouseUp(emberCommon::Input::MouseButton button);
        static bool MouseHeld(emberCommon::Input::MouseButton button);
        static float MouseX();
        static float MouseY();
        static Float2 MousePos();
        static float MouseX01();
        static float MouseY01();
        static Float2 MousePos01();
        static float MouseScrollX();
        static float MouseScrollY();

        // Raw event queries:
        static bool AnyKeyDownRaw();
        static bool AnyKeyUpRaw();
        static bool AnyKeyHeldRaw();
        static bool AnyMouseDownRaw();
        static bool AnyMouseUpRaw();
        static bool AnyMouseHeldRaw();
        static bool KeyDownRaw(emberCommon::Input::Key key);
        static bool KeyUpRaw(emberCommon::Input::Key key);
        static bool KeyHeldRaw(emberCommon::Input::Key key);
        static bool KeyDownOrHeldRaw(emberCommon::Input::Key key);
        static bool MouseDownRaw(emberCommon::Input::MouseButton button);
        static bool MouseUpRaw(emberCommon::Input::MouseButton button);
        static bool MouseHeldRaw(emberCommon::Input::MouseButton button);
        static float MouseScrollXRaw();
        static float MouseScrollYRaw();

        // Event consumption:
        // For ordered input phases (e.g. gui->editor->scene), not arbitrary ECS component conflict resolution.
        static void ConsumeKey(emberCommon::Input::Key key, const std::string& consumer = "");
        static void ConsumeMouseButton(emberCommon::Input::MouseButton button, const std::string& consumer = "");
        static void ConsumeMouseScroll(const std::string& consumer = "");
        static bool KeyConsumed(emberCommon::Input::Key key);
        static bool MouseButtonConsumed(emberCommon::Input::MouseButton button);
        static bool MouseScrollConsumed();
        static const std::string& KeyConsumer(emberCommon::Input::Key key);
        static const std::string& MouseButtonConsumer(emberCommon::Input::MouseButton button);
        static const std::string& MouseScrollConsumer();

    private: // Methods
        static void ClearInputState();
        static void ClearEvents();
        static void ClearConsumedEvents();
        static bool ShouldProcessEvent(const emberCommon::Event& event);
        static bool IsMouseButtonCapturedByGui(emberCommon::Input::MouseButton button);
        static bool AnyMouseButtonCapturedByGui();

        // Delete all constructors:
        EventSystem() = delete;
        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;
        ~EventSystem() = delete;
    };
}