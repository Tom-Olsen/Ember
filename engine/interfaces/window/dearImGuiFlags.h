#pragma once



namespace emberEngine
{
    typedef int DearImGuiWindowFlags;
    enum DearImGuiWindowFlag
    {
        DearImGuiWindowFlag_None                        = 0,
        DearImGuiWindowFlag_NoTitleBar                  = 1 << 0,   // Disable title-bar
        DearImGuiWindowFlag_NoResize                    = 1 << 1,   // Disable user resizing with the lower-right grip
        DearImGuiWindowFlag_NoMove                      = 1 << 2,   // Disable user moving the window
        DearImGuiWindowFlag_NoScrollbar                 = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
        DearImGuiWindowFlag_NoScrollWithMouse           = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
        DearImGuiWindowFlag_NoCollapse                  = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
        DearImGuiWindowFlag_AlwaysAutoResize            = 1 << 6,   // Resize every window to its content every frame
        DearImGuiWindowFlag_NoBackground                = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
        DearImGuiWindowFlag_NoSavedSettings             = 1 << 8,   // Never load/save settings in .ini file
        DearImGuiWindowFlag_NoMouseInputs               = 1 << 9,   // Disable catching mouse, hovering test with pass through.
        DearImGuiWindowFlag_MenuBar                     = 1 << 10,  // Has a menu-bar
        DearImGuiWindowFlag_HorizontalScrollbar         = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
        DearImGuiWindowFlag_NoFocusOnAppearing          = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
        DearImGuiWindowFlag_NoBringToFrontOnFocus       = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        DearImGuiWindowFlag_AlwaysVerticalScrollbar     = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        DearImGuiWindowFlag_AlwaysHorizontalScrollbar   = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        DearImGuiWindowFlag_NoNavInputs                 = 1 << 16,  // No keyboard/gamepad navigation within the window
        DearImGuiWindowFlag_NoNavFocus                  = 1 << 17,  // No focusing toward this window with keyboard/gamepad navigation (e.g. skipped by CTRL+TAB)
        DearImGuiWindowFlag_UnsavedDocument             = 1 << 18,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        DearImGuiWindowFlag_NoDocking                   = 1 << 19,  // Disable docking of this window
        DearImGuiWindowFlag_NoNav                       = DearImGuiWindowFlag_NoNavInputs | DearImGuiWindowFlag_NoNavFocus,
        DearImGuiWindowFlag_NoDecoration                = DearImGuiWindowFlag_NoTitleBar | DearImGuiWindowFlag_NoResize | DearImGuiWindowFlag_NoScrollbar | DearImGuiWindowFlag_NoCollapse,
        DearImGuiWindowFlag_NoInputs                    = DearImGuiWindowFlag_NoMouseInputs | DearImGuiWindowFlag_NoNavInputs | DearImGuiWindowFlag_NoNavFocus
    };

    typedef int DearImGuiFocusedFlags;
    enum DearImGuiFocusedFlag
    {
       DearImGuiFocusedFlag_None                = 0,
       DearImGuiFocusedFlag_ChildWindows        = 1 << 0,   // Return true if any children of the window is focused
       DearImGuiFocusedFlag_RootWindow          = 1 << 1,   // Test from root window (top most parent of the current hierarchy)
       DearImGuiFocusedFlag_AnyWindow           = 1 << 2,   // Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use 'io.WantCaptureMouse' instead! Please read the FAQ!
       DearImGuiFocusedFlag_NoPopupHierarchy    = 1 << 3,   // Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
       DearImGuiFocusedFlag_DockHierarchy       = 1 << 4,   // Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
       DearImGuiFocusedFlag_RootAndChildWindows = DearImGuiFocusedFlag_RootWindow | DearImGuiFocusedFlag_ChildWindows
    };
}