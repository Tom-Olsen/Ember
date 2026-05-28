# GUI, Editor, and Input Routing

This page explains how the GUI backend, editor, window backend, and event system cooperate.

The main idea is dependency direction:

- The engine owns editor windows and editor policy.
- The GUI backend owns ImGui frame lifetime and ImGui capture state.
- The SDL window backend owns platform event polling and decides which SDL events become Ember events.
- The event system stores only events that survived platform and GUI filtering.

## System Boundaries

```mermaid
flowchart LR
    Core[Core::InitBackends] -->|passes callbacks| GuiBackend[IGui backend]
    GuiBackend -->|does not include Editor| EditorCallbacks[Editor callbacks]
    WindowBackend[SDL window backend] -->|SDL event| GuiBackend
    WindowBackend -->|uncaptured events only| EventSystem[EventSystem]
    Editor[Editor] -->|renders| EditorWindow[EditorWindow instances]
    EditorWindow -->|focused and hovered state| Editor
```

`Core::InitBackends` wires the systems together. It gives the GUI backend three editor callbacks:

```cpp
pIGui->SetEditorCallbacks(
    Editor::Render,
    Editor::GetFocusedWindowWantCaptureEvents,
    Editor::GetHoveredWindowWantCaptureEvents
);
```

The backend can then render editor UI and query editor capture policy without depending on the `Editor` class directly.

## Frame Rendering

Editor windows must render inside an active ImGui frame. Today the GUI backend owns that frame lifetime:

```mermaid
sequenceDiagram
    participant App as Application::Run
    participant Gui as Gui::Update
    participant ImGui as ImGui backend
    participant Editor as Editor
    participant Window as EditorWindow

    App->>Gui: Update()
    Gui->>ImGui: NewFrame()
    Gui->>Editor: renderCallback()
    Editor->>Window: RenderWindow()
    Window->>Window: Render()
    Window->>Editor: report focused and hovered state
    Gui->>ImGui: EndFrame()
```

The render callback currently points to `Editor::Render`. That method loops over registered editor windows, sets `s_pCurrentRenderedWindow`, and calls `RenderWindow()` on each window.

## Event Routing

SDL events are filtered before they become Ember events.

```mermaid
sequenceDiagram
    participant App as Application::Run
    participant EventSystem as EventSystem
    participant Window as Window::PollEvents
    participant Gui as Gui backend
    participant Editor as Editor

    App->>EventSystem: ProcessEvents()
    EventSystem->>Window: PollEvents()
    loop each SDL event
        Window->>Gui: ProcessEvent(SDL event)
        Gui->>Editor: keyboard capture callback
        Gui->>Editor: mouse capture callback
        Window->>Gui: WantCaptureKeyboard()
        Window->>Gui: WantCaptureMouse()
        alt GUI captures event
            Window-->>Window: discard SDL event
        else GUI does not capture event
            Window-->>EventSystem: convert to emberCommon::Event
        end
    end
```

The event system does not know which GUI window captured an event. It receives only the events that the SDL window backend decided to convert.

## Capture Policy

ImGui knows whether an active widget wants input. The editor knows whether the current editor window should allow that capture to block scene input.

Those two answers are combined in the GUI backend:

```cpp
m_wantCaptureKeyboard =
    ImGui::WantCaptureKeyboard && Editor::GetFocusedWindowWantCaptureEvents();

m_wantCaptureMouse =
    ImGui::WantCaptureMouse && Editor::GetHoveredWindowWantCaptureEvents();
```

Keyboard uses the focused window because typing belongs to focus. Mouse uses the hovered window because clicking and scrolling belong to the thing under the cursor.

| Input kind | ImGui query | Editor policy query | Reason |
| --- | --- | --- | --- |
| Keyboard | `WantCaptureKeyboard` | focused window wants capture | Text and shortcuts belong to focus |
| Mouse | `WantCaptureMouse` | hovered window wants capture | Clicks and scrolls belong to hover |

## Mouse Button Ownership

Mouse capture is sticky per button. If a mouse button goes down on a GUI-capturing editor window, the SDL window backend keeps that button captured until the matching button-up event.

```mermaid
stateDiagram-v2
    [*] --> Free
    Free --> GuiOwned: mouse down while GUI wants mouse
    Free --> SceneOwned: mouse down while GUI does not want mouse
    GuiOwned --> GuiOwned: motion while held
    SceneOwned --> SceneOwned: motion while held
    GuiOwned --> Free: matching mouse up
    SceneOwned --> Free: matching mouse up
```

This prevents a slider drag from becoming a held scene click after the cursor moves over the viewport.

## Editor Window Policy

Each editor window chooses whether it captures events through `m_wantCaptureEvents`.

| Window type | Typical value | Behavior |
| --- | --- | --- |
| Inspector, console, fluid settings | `true` | UI interaction blocks scene input |
| Scene, game viewport | `false` | Input passes through to world/game systems |

This lets scripts keep using `EventSystem` normally. They do not need one-off checks for GUI capture.

## Important Code Paths

| Responsibility | File |
| --- | --- |
| Backend wiring | `engine/core/src/core.cpp` |
| Editor window registration and capture policy | `engine/core/src/editor/editor.cpp` |
| Per-window focused and hovered reporting | `engine/core/src/editor/editorWindow.cpp` |
| GUI interface contract | `engine/interfaces/gui/iGui.h` |
| ImGui frame and capture state | `engine/backends/imGuiSdlVulkan/src/imGuiSdlVulkan.cpp` |
| SDL event filtering and mouse button ownership | `engine/backends/sdlWindow/src/sdlWindow.cpp` |

## Future Cleanup Option

The capture callbacks are useful because the GUI backend should not depend directly on the editor. The render callback is more optional.

A future GUI frame API could make ownership clearer:

```cpp
Gui::BeginFrame();
Editor::Render();
Gui::EndFrame();
```

That would remove the render callback while preserving the capture policy callbacks.