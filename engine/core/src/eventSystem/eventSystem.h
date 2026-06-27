#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"
#include "commonInput.h"
#include <unordered_map>
#include <unordered_set>



// Foward declarations:
namespace emberCommon
{
    struct Event;
}
namespace emberApplication
{
    class Application;
}



namespace emberCore
{
    // Foward declarations:
    class Editor;
    class EventConsumerScope;



    class EMBER_CORE_API EventSystem
    {
    private: // Friends:
        friend class EventConsumerScope;



    public: // Enums:
        enum class Consumer { none, gui, editor, scene, game };
        enum class KeyState { none, down, up, held };
        enum class MouseState { none, down, up, held };

    private: // Members
        static bool s_isInitialized;
        // Key/MouseButton states:
        static std::unordered_map<emberCommon::Input::Key, KeyState> s_keyStates;
        static std::unordered_map<emberCommon::Input::MouseButton, MouseState> s_mouseButtonStates;
        // MouseButton lock/unlock:
        static std::unordered_map<emberCommon::Input::MouseButton, Consumer> s_lockedMouseButtons;
        static std::unordered_set<emberCommon::Input::MouseButton> s_mouseButtonLocksPendingUnlock;
        // Key/MouseButton consumption:
        static std::unordered_map<emberCommon::Input::Key, Consumer> s_consumedKeys;
        static std::unordered_map<emberCommon::Input::MouseButton, Consumer> s_consumedMouseButtons;
        static Consumer s_currentConsumer;
        static Consumer s_keyboardLockConsumer;
        static Consumer s_mouseScrollConsumer;
        // Basics:
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
        static Float2 MousePos();   // relative to main SDL window, only for game logic, do not use for GUI or editor.
        static float MouseX01();    // relative to main SDL window, only for game logic, do not use for GUI or editor.
        static float MouseY01();    // relative to main SDL window, only for game logic, do not use for GUI or editor.
        static Float2 MousePos01(); // relative to main SDL window, only for game logic, do not use for GUI or editor.
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
        // Marks an input as used for the current frame. For ordered input phases (e.g. gui->editor->scene), not arbitrary ECS component conflict resolution.
        static void ConsumeKey(emberCommon::Input::Key key);
        static void ConsumeMouseButton(emberCommon::Input::MouseButton button);
        static void ConsumeMouseScroll();
        static bool KeyConsumed(emberCommon::Input::Key key);
        static bool MouseButtonConsumed(emberCommon::Input::MouseButton button);
        static bool MouseScrollConsumed();
        static Consumer GetKeyConsumer(emberCommon::Input::Key key);
        static Consumer GetMouseButtonConsumer(emberCommon::Input::MouseButton button);
        static Consumer GetMouseScrollConsumer();

        // Event locking:
        // Locks give one consumer exclusive access across frames, e.g. mouse drags or keyboard focus.
        // The owner can still use normal queries, while other consumers are blocked until unlock/clear.
        static bool TryLockMouseButton(emberCommon::Input::MouseButton button);
        static bool UnlockMouseButton(emberCommon::Input::MouseButton button);
        static bool MouseButtonLocked(emberCommon::Input::MouseButton button);
        static bool MouseButtonLockedBy(emberCommon::Input::MouseButton button, Consumer consumer);
        static Consumer GetMouseButtonLockConsumer(emberCommon::Input::MouseButton button);
        static bool TryLockKeyboard();
        static bool UnlockKeyboard();
        static bool KeyboardLocked();
        static bool KeyboardLockedBy(Consumer consumer);
        static Consumer GetKeyboardLockConsumer();

    private: // Methods
        static void SetCurrentConsumer(Consumer consumer);
        static Consumer GetCurrentConsumer();
        static void ClearInputState();
        static void ClearEvents();
        static void ClearConsumedEvents();
        static void UnlockPendingMouseButtons();
        static bool KeyBlocked(emberCommon::Input::Key key);
        static bool MouseButtonBlocked(emberCommon::Input::MouseButton button);
        static bool MouseScrollBlocked();
        static bool ProcessGuiEventFilter(const emberCommon::Event& event);
        static bool AnyMouseButtonLockedBy(Consumer consumer);

        // Delete all constructors:
        EventSystem() = delete;
        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;
        ~EventSystem() = delete;
    };



    class EMBER_CORE_API EventConsumerScope
    {
    private: // Friends:
        friend class Editor;
        friend class emberApplication::Application;

    private: // Members:
        EventSystem::Consumer m_previousConsumer; // allows nested consumer scopes.

    private: // Methods:
        EventConsumerScope(EventSystem::Consumer consumer);
        ~EventConsumerScope();
    };
}