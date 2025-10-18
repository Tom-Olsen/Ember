#pragma once



namespace emberCommon
{
    typedef int GuiWindowFlags;
    enum GuiWindowFlag
    {
        GuiWindowFlag_None                        = 0,
        GuiWindowFlag_NoTitleBar                  = 1 << 0,   // Disable title-bar
        GuiWindowFlag_NoResize                    = 1 << 1,   // Disable user resizing with the lower-right grip
        GuiWindowFlag_NoMove                      = 1 << 2,   // Disable user moving the window
        GuiWindowFlag_NoScrollbar                 = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
        GuiWindowFlag_NoScrollWithMouse           = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
        GuiWindowFlag_NoCollapse                  = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
        GuiWindowFlag_AlwaysAutoResize            = 1 << 6,   // Resize every window to its content every frame
        GuiWindowFlag_NoBackground                = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
        GuiWindowFlag_NoSavedSettings             = 1 << 8,   // Never load/save settings in .ini file
        GuiWindowFlag_NoMouseInputs               = 1 << 9,   // Disable catching mouse, hovering test with pass through.
        GuiWindowFlag_MenuBar                     = 1 << 10,  // Has a menu-bar
        GuiWindowFlag_HorizontalScrollbar         = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
        GuiWindowFlag_NoFocusOnAppearing          = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
        GuiWindowFlag_NoBringToFrontOnFocus       = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        GuiWindowFlag_AlwaysVerticalScrollbar     = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        GuiWindowFlag_AlwaysHorizontalScrollbar   = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        GuiWindowFlag_NoNavInputs                 = 1 << 16,  // No keyboard/gamepad navigation within the window
        GuiWindowFlag_NoNavFocus                  = 1 << 17,  // No focusing toward this window with keyboard/gamepad navigation (e.g. skipped by CTRL+TAB)
        GuiWindowFlag_UnsavedDocument             = 1 << 18,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        GuiWindowFlag_NoDocking                   = 1 << 19,  // Disable docking of this window
        GuiWindowFlag_NoNav                       = GuiWindowFlag_NoNavInputs | GuiWindowFlag_NoNavFocus,
        GuiWindowFlag_NoDecoration                = GuiWindowFlag_NoTitleBar | GuiWindowFlag_NoResize | GuiWindowFlag_NoScrollbar | GuiWindowFlag_NoCollapse,
        GuiWindowFlag_NoInputs                    = GuiWindowFlag_NoMouseInputs | GuiWindowFlag_NoNavInputs | GuiWindowFlag_NoNavFocus
    };



    typedef int GuiFocusedFlags;
    enum GuiFocusedFlag
    {
       GuiFocusedFlag_None                = 0,
       GuiFocusedFlag_ChildWindows        = 1 << 0,   // Return true if any children of the window is focused
       GuiFocusedFlag_RootWindow          = 1 << 1,   // Test from root window (top most parent of the current hierarchy)
       GuiFocusedFlag_AnyWindow           = 1 << 2,   // Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use 'io.WantCaptureMouse' instead! Please read the FAQ!
       GuiFocusedFlag_NoPopupHierarchy    = 1 << 3,   // Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
       GuiFocusedFlag_DockHierarchy       = 1 << 4,   // Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
       GuiFocusedFlag_RootAndChildWindows = GuiFocusedFlag_RootWindow | GuiFocusedFlag_ChildWindows
    };



    typedef int GuiButtonFlags;
    enum GuiButtonFlag
    {
        GuiButtonFlags_None = 0,
        GuiButtonFlags_MouseButtonLeft = 1 << 0,    // React on left mouse button (default)
        GuiButtonFlags_MouseButtonRight = 1 << 1,   // React on right mouse button
        GuiButtonFlags_MouseButtonMiddle = 1 << 2,  // React on center mouse button
        GuiButtonFlags_MouseButtonMask = GuiButtonFlags_MouseButtonLeft | GuiButtonFlags_MouseButtonRight | GuiButtonFlags_MouseButtonMiddle, // [Internal]
        GuiButtonFlags_EnableNav = 1 << 3,          // InvisibleButton(): do not disable navigation/tabbing. Otherwise disabled by default.
    };



    typedef int GuiInputTextFlags;
    enum GuiInputTextFlag
    {
        // Basic filters (also see ImGuiInputTextFlags_CallbackCharFilter):
        GuiInputTextFlags_None = 0,
        GuiInputTextFlags_CharsDecimal = 1 << 0,        // Allow 0123456789.+-*/
        GuiInputTextFlags_CharsHexadecimal = 1 << 1,    // Allow 0123456789ABCDEFabcdef
        GuiInputTextFlags_CharsScientific = 1 << 2,     // Allow 0123456789.+-*/eE (Scientific notation input)
        GuiInputTextFlags_CharsUppercase = 1 << 3,      // Turn a..z into A..Z
        GuiInputTextFlags_CharsNoBlank = 1 << 4,        // Filter out spaces, tabs

        // Inputs:
        GuiInputTextFlags_AllowTabInput = 1 << 5,       // Pressing TAB input a '\t' character into the text field
        GuiInputTextFlags_EnterReturnsTrue = 1 << 6,    // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider using IsItemDeactivatedAfterEdit() instead!
        GuiInputTextFlags_EscapeClearsAll = 1 << 7,     // Escape key clears content if not empty, and deactivate otherwise (contrast to default behavior of Escape to revert)
        GuiInputTextFlags_CtrlEnterForNewLine = 1 << 8, // In multi-line mode, validate with Enter, add new line with Ctrl+Enter (default is opposite: validate with Ctrl+Enter, add line with Enter).

        // Other options:
        GuiInputTextFlags_ReadOnly = 1 << 9,            // Read-only mode
        GuiInputTextFlags_Password = 1 << 10,           // Password mode, display all characters as '*', disable copy
        GuiInputTextFlags_AlwaysOverwrite = 1 << 11,    // Overwrite mode
        GuiInputTextFlags_AutoSelectAll = 1 << 12,      // Select entire text when first taking mouse focus
        GuiInputTextFlags_ParseEmptyRefVal = 1 << 13,   // InputFloat(), InputInt(), InputScalar() etc. only: parse empty string as zero value.
        GuiInputTextFlags_DisplayEmptyRefVal = 1 << 14, // InputFloat(), InputInt(), InputScalar() etc. only: when value is zero, do not display it. Generally used with ImGuiInputTextFlags_ParseEmptyRefVal.
        GuiInputTextFlags_NoHorizontalScroll = 1 << 15, // Disable following the cursor horizontally
        GuiInputTextFlags_NoUndoRedo = 1 << 16,         // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().

        // Elide display / Alignment:
        GuiInputTextFlags_ElideLeft = 1 << 17,  // When text doesn't fit, elide left side to ensure right side stays visible. Useful for path/filenames. Single-line only!

        // Callback features:
        GuiInputTextFlags_CallbackCompletion = 1 << 18, // Callback on pressing TAB (for completion handling)
        GuiInputTextFlags_CallbackHistory = 1 << 19,    // Callback on pressing Up/Down arrows (for history handling)
        GuiInputTextFlags_CallbackAlways = 1 << 20,     // Callback on each iteration. User code may query cursor position, modify text buffer.
        GuiInputTextFlags_CallbackCharFilter = 1 << 21, // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
        GuiInputTextFlags_CallbackResize = 1 << 22,     // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
        GuiInputTextFlags_CallbackEdit = 1 << 23,       // Callback on any edit. Note that InputText() already returns true on edit + you can always use IsItemEdited(). The callback is useful to manipulate the underlying buffer while focus is active.
    };



    typedef int GuiMouseButtons;
    enum GuiMouseButton
    {
        GuiMouseButton_Left = 0,
        GuiMouseButton_Right = 1,
        GuiMouseButton_Middle = 2,
        GuiMouseButton_COUNT = 5
    };



    typedef int GuiDirs;
    enum GuiDir : int
    {
        GuiDir_None = -1,
        GuiDir_Left = 0,
        GuiDir_Right = 1,
        GuiDir_Up = 2,
        GuiDir_Down = 3,
        GuiDir_COUNT
    };



    typedef int GuiTreeNodeFlags;
    enum GuiTreeNodeFlag
    {
        GuiTreeNodeFlags_None                 = 0,
        GuiTreeNodeFlags_Selected             = 1 << 0,   // Draw as selected
        GuiTreeNodeFlags_Framed               = 1 << 1,   // Draw frame with background (e.g. for CollapsingHeader)
        GuiTreeNodeFlags_AllowOverlap         = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
        GuiTreeNodeFlags_NoTreePushOnOpen     = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
        GuiTreeNodeFlags_NoAutoOpenOnLog      = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
        GuiTreeNodeFlags_DefaultOpen          = 1 << 5,   // Default node to be open
        GuiTreeNodeFlags_OpenOnDoubleClick    = 1 << 6,   // Open on double-click instead of simple click (default for multi-select unless any _OpenOnXXX behavior is set explicitly). Both behaviors may be combined.
        GuiTreeNodeFlags_OpenOnArrow          = 1 << 7,   // Open when clicking on the arrow part (default for multi-select unless any _OpenOnXXX behavior is set explicitly). Both behaviors may be combined.
        GuiTreeNodeFlags_Leaf                 = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
        GuiTreeNodeFlags_Bullet               = 1 << 9,   // Display a bullet instead of arrow. IMPORTANT: node can still be marked open/close if you don't set the _Leaf flag!
        GuiTreeNodeFlags_FramePadding         = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding() before the node.
        GuiTreeNodeFlags_SpanAvailWidth       = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line without using AllowOverlap mode.
        GuiTreeNodeFlags_SpanFullWidth        = 1 << 12,  // Extend hit box to the left-most and right-most edges (cover the indent area).
        GuiTreeNodeFlags_SpanLabelWidth       = 1 << 13,  // Narrow hit box + narrow hovering highlight, will only cover the label text.
        GuiTreeNodeFlags_SpanAllColumns       = 1 << 14,  // Frame will span all columns of its container table (label will still fit in current column)
        GuiTreeNodeFlags_LabelSpanAllColumns  = 1 << 15,  // Label will span all columns of its container table
        //GuiTreeNodeFlags_NoScrollOnOpen     = 1 << 16,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
        GuiTreeNodeFlags_NavLeftJumpsToParent = 1 << 17,  // Nav: left arrow moves back to parent. This is processed in TreePop() when there's an unfullfilled Left nav request remaining.
        GuiTreeNodeFlags_CollapsingHeader     = GuiTreeNodeFlags_Framed | GuiTreeNodeFlags_NoTreePushOnOpen | GuiTreeNodeFlags_NoAutoOpenOnLog,
    
        // [EXPERIMENTAL] Draw lines connecting TreeNode hierarchy. Discuss in GitHub issue #2920.
        // Default value is pulled from style.TreeLinesFlags. May be overridden in TreeNode calls.
        GuiTreeNodeFlags_DrawLinesNone        = 1 << 18,  // No lines drawn
        GuiTreeNodeFlags_DrawLinesFull        = 1 << 19,  // Horizontal lines to child nodes. Vertical line drawn down to TreePop() position: cover full contents. Faster (for large trees).
        GuiTreeNodeFlags_DrawLinesToNodes     = 1 << 20,  // Horizontal lines to child nodes. Vertical line drawn down to bottom-most child node. Slower (for large trees).
    };


    
    typedef int GuiHoveredFlags;
    enum GuiHoveredFlag
    {
        GuiHoveredFlags_None                          = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
        GuiHoveredFlags_ChildWindows                  = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
        GuiHoveredFlags_RootWindow                    = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
        GuiHoveredFlags_AnyWindow                     = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
        GuiHoveredFlags_NoPopupHierarchy              = 1 << 3,   // IsWindowHovered() only: Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
        GuiHoveredFlags_DockHierarchy                 = 1 << 4,   // IsWindowHovered() only: Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
        GuiHoveredFlags_AllowWhenBlockedByPopup       = 1 << 5,   // Return true even if a popup window is normally blocking access to this item/window
        //GuiHoveredFlags_AllowWhenBlockedByModal     = 1 << 6,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
        GuiHoveredFlags_AllowWhenBlockedByActiveItem  = 1 << 7,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
        GuiHoveredFlags_AllowWhenOverlappedByItem     = 1 << 8,   // IsItemHovered() only: Return true even if the item uses AllowOverlap mode and is overlapped by another hoverable item.
        GuiHoveredFlags_AllowWhenOverlappedByWindow   = 1 << 9,   // IsItemHovered() only: Return true even if the position is obstructed or overlapped by another window.
        GuiHoveredFlags_AllowWhenDisabled             = 1 << 10,  // IsItemHovered() only: Return true even if the item is disabled
        GuiHoveredFlags_NoNavOverride                 = 1 << 11,  // IsItemHovered() only: Disable using keyboard/gamepad navigation state when active, always query mouse
        GuiHoveredFlags_AllowWhenOverlapped           = GuiHoveredFlags_AllowWhenOverlappedByItem | GuiHoveredFlags_AllowWhenOverlappedByWindow,
        GuiHoveredFlags_RectOnly                      = GuiHoveredFlags_AllowWhenBlockedByPopup | GuiHoveredFlags_AllowWhenBlockedByActiveItem | GuiHoveredFlags_AllowWhenOverlapped,
        GuiHoveredFlags_RootAndChildWindows           = GuiHoveredFlags_RootWindow | GuiHoveredFlags_ChildWindows,
    
        // Tooltips mode
        // - typically used in IsItemHovered() + SetTooltip() sequence.
        // - this is a shortcut to pull flags from 'style.HoverFlagsForTooltipMouse' or 'style.HoverFlagsForTooltipNav' where you can reconfigure desired behavior.
        //   e.g. 'TooltipHoveredFlagsForMouse' defaults to 'ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort'.
        // - for frequently actioned or hovered items providing a tooltip, you want may to use ImGuiHoveredFlags_ForTooltip (stationary + delay) so the tooltip doesn't show too often.
        // - for items which main purpose is to be hovered, or items with low affordance, or in less consistent apps, prefer no delay or shorter delay.
        GuiHoveredFlags_ForTooltip                    = 1 << 12,  // Shortcut for standard flags when using IsItemHovered() + SetTooltip() sequence.
    
        // (Advanced) Mouse Hovering delays.
        // - generally you can use ImGuiHoveredFlags_ForTooltip to use application-standardized flags.
        // - use those if you need specific overrides.
        GuiHoveredFlags_Stationary                    = 1 << 13,  // Require mouse to be stationary for style.HoverStationaryDelay (~0.15 sec) _at least one time_. After this, can move on same item/window. Using the stationary test tends to reduces the need for a long delay.
        GuiHoveredFlags_DelayNone                     = 1 << 14,  // IsItemHovered() only: Return true immediately (default). As this is the default you generally ignore this.
        GuiHoveredFlags_DelayShort                    = 1 << 15,  // IsItemHovered() only: Return true after style.HoverDelayShort elapsed (~0.15 sec) (shared between items) + requires mouse to be stationary for style.HoverStationaryDelay (once per item).
        GuiHoveredFlags_DelayNormal                   = 1 << 16,  // IsItemHovered() only: Return true after style.HoverDelayNormal elapsed (~0.40 sec) (shared between items) + requires mouse to be stationary for style.HoverStationaryDelay (once per item).
        GuiHoveredFlags_NoSharedDelay                 = 1 << 17,  // IsItemHovered() only: Disable shared delay system where moving from one item to the next keeps the previous timer for a short time (standard for tooltips with long delays)
    };
}