#pragma once
#include "emberMath.h"



namespace emberCommon
{
	struct GuiStyle
	{
		// Compact backend-neutral style snapshot for layout decisions.
		float fontSize = 0.0f;                          // Current base font size before external global factors are applied.
		float alpha = 1.0f;                             // Global alpha applies to everything in Dear ImGui.
		Float2 windowPadding;                           // Padding within a window.
		Float2 framePadding;                            // Padding within a framed rectangle (used by most widgets).
		Float2 itemSpacing;                             // Horizontal and vertical spacing between widgets/lines.
		Float2 itemInnerSpacing;                        // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
		Float2 cellPadding;                             // Padding within a table cell. Cellpadding.x is locked for entire table. CellPadding.y may be altered between different rows.
		float indentSpacing = 0.0f;                     // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
		float scrollbarSize = 0.0f;                     // Width of the vertical scrollbar, Height of the horizontal scrollbar.
		float grabMinSize = 0.0f;                       // Minimum width/height of a grab box for slider/scrollbar.
		float windowRounding = 0.0f;                    // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
		float frameRounding = 0.0f;                     // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
		Float2 windowTitleTextAlign;		            // Normalized text alignment in the title bar, from top-left 0 to bottom-right 1.
		Float2 buttonTextAlign;			                // Normalized text alignment inside buttons larger than their label.
		Float2 selectableTextAlign;		                // Normalized text alignment for selectable rows/items.
		Float2 separatorTextAlign;		                // Normalized label alignment for text separators.
		Float2 separatorTextPadding;		            // Extra spacing around separator text.
		Float2 tableAngledHeaderTextAlign;	            // Normalized label alignment for angled table headers.
		float selectedTabCloseButtonMinWidth = -1.0f;	// <0 always show, 0 hover-only, >0 hover-only when the tab is wide enough.
		float unselectedTabCloseButtonMinWidth = 0.0f;	// <0 always show, 0 hover-only, >0 hover-only when the tab is wide enough.

		// Skipped for now:
		// full color table: large backend-specific enum space; expose semantic editor colors instead.
		// docking/table/tree style knobs: add focused fields when Ember's editor exposes those widgets directly.
		// anti-aliasing/tessellation controls: renderer/backend implementation detail.
		// tooltip hover delays/nav style: needs a higher-level tooltip policy API first.
	};
}