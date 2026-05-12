#pragma once
#include <cstdint>



namespace emberCommon
{
	enum class GuiWindowFlags : uint32_t
	{
		none = 0,
		noTitleBar = 1 << 0,			// Hide the title bar for tool overlays or fixed editor panes.
		noResize = 1 << 1,				// Prevent user resizing when layout is controlled by code/docking.
		noMove = 1 << 2,				// Prevent user moving when layout is controlled by code/docking.
		noScrollbar = 1 << 3,			// Hide scrollbars while still allowing programmatic layout.
		noScrollWithMouse = 1 << 4,		// Prevent mouse-wheel scrolling inside the window.
		noCollapse = 1 << 5,			// Prevent folding the window into only its title bar.
		noBackground = 1 << 6,			// Useful for transparent overlays.
		noSavedSettings = 1 << 7,		// Do not persist position/size in backend-specific settings.
		menuBar = 1 << 8,				// Reserve space for a menu bar at the top of the window.
		noDocking = 1 << 9				// Keep this window out of editor docking layouts.

		// Skipped for now:
		// alwaysAutoResize: useful for popups/tooltips, but usually bad for persistent editor windows.
		// noMouseInputs/noNavInputs/noNavFocus/noInputs: input routing policy should stay explicit before exposing these.
		// horizontalScrollbar/always scrollbars: add when an editor window needs table-like horizontal navigation.
		// noFocusOnAppearing/noBringToFrontOnFocus: add when focus/stacking behavior becomes a real editor workflow issue.
		// unsavedDocument: useful for document tabs once the editor has save/dirty-state semantics.
		// composite aliases such as noDecoration/noInputs: easy to rebuild locally with operator|.
	};

	enum class GuiFocusedFlags : uint32_t
	{
		none = 0,
		childWindows = 1 << 0,			// Treat focused child windows as focus on the parent.
		rootWindow = 1 << 1,            // Test focus from the root of the current window hierarchy.
		anyWindow = 1 << 2,				// True when any GUI window is focused.
		rootAndChildWindows = (1 << 0) | (1 << 1)

		// Skipped for now:
		// noPopupHierarchy/dockHierarchy: backend-specific hierarchy rules; expose only when editor focus bugs require them.
	};

	enum class GuiButtonFlags : uint32_t
	{
		none = 0,
		mouseButtonLeft = 1 << 0,		// Trigger an invisible/button-like hit target from the left mouse button.
		mouseButtonRight = 1 << 1,		// Trigger from the right mouse button, useful for context actions.
		mouseButtonMiddle = 1 << 2		// Trigger from the middle mouse button, useful for viewport/editor tools.

		// Skipped for now:
		// keyboard/gamepad navigation overrides: expose later if invisible widgets need to participate in nav focus.
		// internal mouse masks: backend implementation detail, not an engine API concept.
	};

	enum class GuiInputTextFlags : uint32_t
	{
		none = 0,
		charsDecimal = 1 << 0,			// Allow decimal number input only.
		charsHexadecimal = 1 << 1,		// Allow hexadecimal number input only.
		charsScientific = 1 << 2,		// Allow scientific notation input.
		charsUppercase = 1 << 3,		// Convert typed characters to uppercase.
		charsNoBlank = 1 << 4,			// Reject spaces and tabs.
		allowTabInput = 1 << 5,			// Insert tab characters instead of moving focus, useful for multiline editors.
		enterReturnsTrue = 1 << 6,		// Report changes when Enter confirms the edit.
		escapeClearsAll = 1 << 7,		// Pressing Escape clears non-empty text and otherwise deactivates the field.
		ctrlEnterForNewLine = 1 << 8,	// In multiline text, use Ctrl+Enter for newline and Enter for confirmation.
		readOnly = 1 << 9,				// Display selectable text without accepting edits.
		password = 1 << 10,				// Hide visible characters for sensitive values.
		alwaysOverwrite = 1 << 11,		// Replace characters under the cursor instead of inserting before them.
		autoSelectAll = 1 << 12,			// Select all text when the field first receives focus.
		noHorizontalScroll = 1 << 13,	// Keep text fixed horizontally instead of tracking the cursor.
		noUndoRedo = 1 << 14				// Disable the backend text edit undo/redo stack.

		// Skipped for now:
		// parse/display-empty-reference-value: numeric backend parsing detail.
		// elide-left: useful for path fields; add once path widgets exist.
		// callbacks: require callback data types, so they need a dedicated engine-level text edit API.
	};

	enum class GuiMouseButton : uint8_t
	{
		left,
		right,
		middle

		// Skipped for now:
		// extra mouse buttons and button counts. Add named buttons only when editor tools consume them.
	};



    // GuiWindowFlags operator overloading:
	inline GuiWindowFlags operator|(GuiWindowFlags lhs, GuiWindowFlags rhs)
	{
		return static_cast<GuiWindowFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}
	inline GuiWindowFlags operator&(GuiWindowFlags lhs, GuiWindowFlags rhs)
	{
		return static_cast<GuiWindowFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
	}
	inline GuiWindowFlags& operator|=(GuiWindowFlags& lhs, GuiWindowFlags rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}
	[[nodiscard]] inline bool HasFlag(GuiWindowFlags flags, GuiWindowFlags flag)
	{
		return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
	}

    // GuiFocusedFlags operator overloading:
	inline GuiFocusedFlags operator|(GuiFocusedFlags lhs, GuiFocusedFlags rhs)
	{
		return static_cast<GuiFocusedFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}
	inline GuiFocusedFlags operator&(GuiFocusedFlags lhs, GuiFocusedFlags rhs)
	{
		return static_cast<GuiFocusedFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
	}
	inline GuiFocusedFlags& operator|=(GuiFocusedFlags& lhs, GuiFocusedFlags rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}
	[[nodiscard]] inline bool HasFlag(GuiFocusedFlags flags, GuiFocusedFlags flag)
	{
		return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
	}

    // GuiButtonFlags operator overloading:
	inline GuiButtonFlags operator|(GuiButtonFlags lhs, GuiButtonFlags rhs)
	{
		return static_cast<GuiButtonFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}
	inline GuiButtonFlags operator&(GuiButtonFlags lhs, GuiButtonFlags rhs)
	{
		return static_cast<GuiButtonFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
	}
	inline GuiButtonFlags& operator|=(GuiButtonFlags& lhs, GuiButtonFlags rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}
	[[nodiscard]] inline bool HasFlag(GuiButtonFlags flags, GuiButtonFlags flag)
	{
		return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
	}

    // GuiInputTextFlags operator overloading:
	inline GuiInputTextFlags operator|(GuiInputTextFlags lhs, GuiInputTextFlags rhs)
	{
		return static_cast<GuiInputTextFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}
	inline GuiInputTextFlags operator&(GuiInputTextFlags lhs, GuiInputTextFlags rhs)
	{
		return static_cast<GuiInputTextFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
	}
	inline GuiInputTextFlags& operator|=(GuiInputTextFlags& lhs, GuiInputTextFlags rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}
	[[nodiscard]] inline bool HasFlag(GuiInputTextFlags flags, GuiInputTextFlags flag)
	{
		return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
	}
}