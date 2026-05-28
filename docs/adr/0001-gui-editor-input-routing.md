# ADR 0001: GUI Editor Input Routing

## Status

Accepted

## Context

Editor windows and scene systems both read input. A click on a GUI control must not also trigger world interaction, such as the fluid attractor. At the same time, scene and game viewport windows must allow input to pass through.

The GUI backend can see ImGui capture state, but it should not depend directly on the engine editor. The editor can see which editor window is focused or hovered, and each editor window owns its `m_wantCaptureEvents` policy.

## Decision

The GUI backend receives editor callbacks during backend initialization:

- render editor windows
- query focused-window capture policy for keyboard input
- query hovered-window capture policy for mouse input

Keyboard capture combines ImGui keyboard capture with focused editor-window policy.

Mouse capture combines ImGui mouse capture with hovered editor-window policy.

The SDL window backend filters captured SDL events before converting them into `emberCommon::Event` values. It also tracks GUI-owned mouse buttons from button-down until button-up.

## Consequences

Scripts can continue to read `EventSystem` without checking GUI state.

Editor windows express pass-through or capture behavior with `m_wantCaptureEvents`.

The GUI backend stays independent from `emberEngine::Editor`.

Mouse drags that start in captured GUI remain captured until release.

## Alternatives Considered

Adding local GUI checks inside scene scripts was rejected because it spreads input-routing policy into gameplay or simulation code.

Using only focused-window capture for mouse was rejected because the first click into a GUI window can be routed incorrectly, and mouse input semantically belongs to hover.

Making the GUI backend include the editor directly was rejected because it would couple backend code to engine editor code.

## Follow-Up

Consider replacing the editor render callback with explicit GUI frame boundaries:

```cpp
Gui::BeginFrame();
Editor::Render();
Gui::EndFrame();
```

Keep capture policy separated from frame rendering, because event filtering still needs backend-to-editor policy queries.