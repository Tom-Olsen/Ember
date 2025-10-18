#pragma once
#include "emberMath.h"
#include "commonGuiFlags.h"



namespace emberCommon
{
    // Copy of ImGuiStyle struct form Dear ImGui:
    static constexpr int GuiColCount = 60; // must match ImGuiCol_COUNT in the ImGui backend version.
    struct GuiStyle
    {
        // Font scaling
        // - recap: ImGui::GetFontSize() == FontSizeBase * (FontScaleMain * FontScaleDpi * other_scaling_factors)
        float               FontSizeBase;                       // Current base font size before external global factors are applied. Use PushFont(NULL, size) to modify. Use ImGui::GetFontSize() to obtain scaled value.
        float               FontScaleMain;                      // Main global scale factor. May be set by application once, or exposed to end-user.
        float               FontScaleDpi;                       // Additional global scale factor from viewport/monitor contents scale. When io.ConfigDpiScaleFonts is enabled, this is automatically overwritten when changing monitor DPI.
        float               Alpha;                              // Global alpha applies to everything in Dear ImGui.
        float               DisabledAlpha;                      // Additional alpha multiplier applied by BeginDisabled(). Multiply over current value of Alpha.
        Float2              WindowPadding;                      // Padding within a window.
        float               WindowRounding;                     // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
        float               WindowBorderSize;                   // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        float               WindowBorderHoverPadding;           // Hit-testing extent outside/inside resizing border. Also extend determination of hovered window. Generally meaningfully larger than WindowBorderSize to make it easy to reach borders.
        Float2              WindowMinSize;                      // Minimum window size. This is a global setting. If you want to constrain individual windows, use SetNextWindowSizeConstraints().
        Float2              WindowTitleAlign;                   // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
        GuiDir              WindowMenuButtonPosition;           // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to ImGuiDir_Left.
        float               ChildRounding;                      // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
        float               ChildBorderSize;                    // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        float               PopupRounding;                      // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
        float               PopupBorderSize;                    // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        Float2              FramePadding;                       // Padding within a framed rectangle (used by most widgets).
        float               FrameRounding;                      // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
        float               FrameBorderSize;                    // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
        Float2              ItemSpacing;                        // Horizontal and vertical spacing between widgets/lines.
        Float2              ItemInnerSpacing;                   // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
        Float2              CellPadding;                        // Padding within a table cell. Cellpadding.x is locked for entire table. CellPadding.y may be altered between different rows.
        Float2              TouchExtraPadding;                  // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
        float               IndentSpacing;                      // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
        float               ColumnsMinSpacing;                  // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
        float               ScrollbarSize;                      // Width of the vertical scrollbar, Height of the horizontal scrollbar.
        float               ScrollbarRounding;                  // Radius of grab corners for scrollbar.
        float               ScrollbarPadding;                   // Padding of scrollbar grab within its frame (same for both axises).
        float               GrabMinSize;                        // Minimum width/height of a grab box for slider/scrollbar.
        float               GrabRounding;                       // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
        float               LogSliderDeadzone;                  // The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero.
        float               ImageBorderSize;                    // Thickness of border around Image() calls.
        float               TabRounding;                        // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
        float               TabBorderSize;                      // Thickness of border around tabs.
        float               TabMinWidthBase;                    // Minimum tab width, to make tabs larger than their contents. TabBar buttons are not affected.
        float               TabMinWidthShrink;                  // Minimum tab width after shrinking, when using ImGuiTabBarFlags_FittingPolicyMixed policy.
        float               TabCloseButtonMinWidthSelected;     // -1: always visible. 0.0f: visible when hovered. >0.0f: visible when hovered if minimum width.
        float               TabCloseButtonMinWidthUnselected;   // -1: always visible. 0.0f: visible when hovered. >0.0f: visible when hovered if minimum width. FLT_MAX: never show close button when unselected.
        float               TabBarBorderSize;                   // Thickness of tab-bar separator, which takes on the tab active color to denote focus.
        float               TabBarOverlineSize;                 // Thickness of tab-bar overline, which highlights the selected tab-bar.
        float               TableAngledHeadersAngle;            // Angle of angled headers (supported values range from -50.0f degrees to +50.0f degrees).
        Float2              TableAngledHeadersTextAlign;        // Alignment of angled headers within the cell
        GuiTreeNodeFlags    TreeLinesFlags;                     // Default way to draw lines connecting TreeNode hierarchy. ImGuiTreeNodeFlags_DrawLinesNone or ImGuiTreeNodeFlags_DrawLinesFull or ImGuiTreeNodeFlags_DrawLinesToNodes.
        float               TreeLinesSize;                      // Thickness of outlines when using ImGuiTreeNodeFlags_DrawLines.
        float               TreeLinesRounding;                  // Radius of lines connecting child nodes to the vertical line.
        GuiDir              ColorButtonPosition;                // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
        Float2              ButtonTextAlign;                    // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
        Float2              SelectableTextAlign;                // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
        float               SeparatorTextBorderSize;            // Thickness of border in SeparatorText()
        Float2              SeparatorTextAlign;                 // Alignment of text within the separator. Defaults to (0.0f, 0.5f) (left aligned, center).
        Float2              SeparatorTextPadding;               // Horizontal offset of text from each edge of the separator + spacing on other axis. Generally small values. .y is recommended to be == FramePadding.y.
        Float2              DisplayWindowPadding;               // Apply to regular windows: amount which we enforce to keep visible when moving near edges of your screen.
        Float2              DisplaySafeAreaPadding;             // Apply to every windows, menus, popups, tooltips: amount where we avoid displaying contents. Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).
        float               DockingSeparatorSize;               // Thickness of resizing border between docked windows
        float               MouseCursorScale;                   // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). We apply per-monitor DPI scaling over this scale. May be removed later.
        bool                AntiAliasedLines;                   // Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
        bool                AntiAliasedLinesUseTex;             // Enable anti-aliased lines/borders using textures where possible. Require backend to render with bilinear filtering (NOT point/nearest filtering). Latched at the beginning of the frame (copied to ImDrawList).
        bool                AntiAliasedFill;                    // Enable anti-aliased edges around filled shapes (rounded rectangles, circles, etc.). Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
        float               CurveTessellationTol;               // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
        float               CircleTessellationMaxError;         // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.
        // Colors
        Float4              Colors[GuiColCount];
        // Behaviors
        // (It is possible to modify those fields mid-frame if specific behavior need it, unlike e.g. configuration fields in ImGuiIO)
        float               HoverStationaryDelay;               // Delay for IsItemHovered(ImGuiHoveredFlags_Stationary). Time required to consider mouse stationary.
        float               HoverDelayShort;                    // Delay for IsItemHovered(ImGuiHoveredFlags_DelayShort). Usually used along with HoverStationaryDelay.
        float               HoverDelayNormal;                   // Delay for IsItemHovered(ImGuiHoveredFlags_DelayNormal). "
        GuiHoveredFlags     HoverFlagsForTooltipMouse;          // Default flags when using IsItemHovered(ImGuiHoveredFlags_ForTooltip) or BeginItemTooltip()/SetItemTooltip() while using mouse.
        GuiHoveredFlags     HoverFlagsForTooltipNav;            // Default flags when using IsItemHovered(ImGuiHoveredFlags_ForTooltip) or BeginItemTooltip()/SetItemTooltip() while using keyboard/gamepad.
    };
}