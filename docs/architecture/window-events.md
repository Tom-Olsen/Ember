# Window Events

This page explains how Ember receives platform window events and turns them into engine input state.

## Responsibilities

| System | Responsibility |
| --- | --- |
| SDL | Produces raw platform events |
| `sdlWindowBackend::Window` | Polls SDL, lets GUI process raw events, filters captured events, converts surviving events |
| GUI backend | Updates ImGui state from raw SDL events and reports capture state |
| `emberEngine::EventSystem` | Converts `emberCommon::Event` values into queryable input state |
| Gameplay, tools, simulations | Read stable input through `EventSystem` |

## Event Pipeline

```mermaid
flowchart TD
    App[Application::Run] --> EventSystemProcess[EventSystem::ProcessEvents]
    EventSystemProcess --> WindowPoll[Window::PollEvents]
    WindowPoll --> SDLPoll[SDL_PollEvent]
    SDLPoll --> GuiProcess[Gui::ProcessEvent]
    GuiProcess --> Filter{Captured by GUI?}
    Filter -->|yes| Skip[Skip SDL event]
    Filter -->|no| Convert[Translate to emberCommon::Event]
    Convert --> Store[Store in Window::m_events]
    Store --> EventSystemUpdate[EventSystem updates key, mouse, window state]
    EventSystemUpdate --> Queries[MouseDown, MouseHeld, MouseScrollY, KeyDown, etc.]
```

Every SDL event is first passed to the GUI backend. This lets ImGui update its internal state before Ember decides whether the event should also become an engine event.

## Polling Loop

The high-level frame path is:

```mermaid
sequenceDiagram
    participant App as Application::Run
    participant EventSystem as EventSystem
    participant Window as Window::PollEvents
    participant SDL as SDL
    participant Gui as GUI backend

    App->>EventSystem: ProcessEvents()
    EventSystem->>EventSystem: ClearEvents()
    EventSystem->>Window: PollEvents()
    loop each SDL event
        Window->>SDL: SDL_PollEvent()
        Window->>Gui: ProcessEvent(SDL event)
        Window->>Gui: WantCaptureKeyboard()
        Window->>Gui: WantCaptureMouse()
        alt captured by GUI
            Window-->>Window: continue
        else engine event
            Window-->>Window: translate SDL event
            Window-->>EventSystem: return emberCommon::Event
        end
    end
```

`EventSystem::ClearEvents()` runs before polling. It transitions previous-frame `down` states to `held`, clears previous-frame `up` states, and resets scroll deltas.

## Filtering Switch

`Window::PollEvents()` first decides whether an SDL event should be swallowed by GUI/editor input handling.

```mermaid
flowchart LR
    Raw[SDL event] --> GuiFirst[GUI processes raw event]
    GuiFirst --> Filter{Filtering switch}
    Filter -->|captured| Discard[Do not create Ember event]
    Filter -->|not captured| Translate[Translation switch]
```

Captured events stop at the filtering switch. They do not reach the second switch and never enter the engine event list.

Mouse capture is based on two ideas:

- current GUI capture state for simple click, wheel, and hover interactions
- per-button event target state for drags that started in GUI

Keyboard capture uses the focused editor window policy. Mouse capture uses the hovered editor window policy.

## Mouse Button Targets

Mouse buttons can target either the engine event system or GUI.

```mermaid
stateDiagram-v2
    [*] --> EventSystem
    EventSystem --> Gui: button down while GUI wants mouse
    Gui --> Gui: motion while held
    Gui --> EventSystem: matching button up
    Gui --> EventSystem: window focus lost
```

The default target is `eventSystem`. If a button-down starts on GUI, that button targets `gui` until the matching button-up. This keeps a GUI drag from becoming a scene drag if the cursor moves over the viewport.

## Translation Switch

Only uncaptured SDL events reach the translation switch.

| SDL event kind | Ember event result |
| --- | --- |
| Quit | `EventType::Quit` |
| Window close | `EventType::WindowClose` |
| Window resize or pixel size change | `EventType::WindowResized` |
| Window minimized | `EventType::WindowMinimized` |
| Window restored | `EventType::WindowRestored` |
| Window focus gained | `EventType::WindowFocusGained` |
| Window focus lost | `EventType::WindowFocusLost` |
| Key down/up | `EventType::KeyDown` / `EventType::KeyUp` |
| Text input | `EventType::TextInput` |
| Mouse motion | `EventType::MouseMoved` |
| Mouse button down/up | `EventType::MouseButtonDown` / `EventType::MouseButtonUp` |
| Mouse wheel | `EventType::MouseWheel` |
| Gamepad events | Controller event types |

Translation also maps backend-specific values into Ember enums, such as SDL mouse buttons into `emberCommon::Input::MouseButton`.

## EventSystem State

After `Window::PollEvents()` returns, `EventSystem::ProcessEvents()` applies each `emberCommon::Event` to engine state.

```mermaid
flowchart TD
    Event[emberCommon::Event] --> Type{EventType}
    Type -->|MouseButtonDown| MouseDown[mouse state = down]
    Type -->|MouseButtonUp| MouseUp[mouse state = up]
    Type -->|MouseMoved| MousePos[update mouse position]
    Type -->|MouseWheel| MouseScroll[update scroll delta]
    Type -->|KeyDown| KeyDown[key state = down]
    Type -->|KeyUp| KeyUp[key state = up]
    Type -->|WindowResized| Resize[update cached window size]
```

User code should read input from `EventSystem`, not directly from SDL or the GUI backend.

## Important Code Paths

| Responsibility | File |
| --- | --- |
| Application frame loop | `engine/applications/emberEditorApp/src/application.cpp` |
| Event system polling and state updates | `engine/core/src/eventSystem/eventSystem.cpp` |
| Window backend event polling and filtering | `engine/backends/sdlWindow/src/sdlWindow.cpp` |
| SDL to Ember enum translation | `engine/backends/sdlWindow/src/sdlEventTranslation.h` |
| Shared event representation | `engine/common/commonEvent.h` |
| Shared input enums | `engine/common/commonInput.h` |