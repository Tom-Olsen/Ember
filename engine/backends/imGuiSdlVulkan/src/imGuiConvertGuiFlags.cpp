#include "imGuiConvertGuiFlags.h"



namespace imGuiSdlVulkanBackend
{
	ImGuiWindowFlags GuiWindowFlagsCommonToImGui(emberCommon::GuiWindowFlags flags)
	{
		ImGuiWindowFlags result = 0;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noTitleBar))
			result |= ImGuiWindowFlags_NoTitleBar;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noResize))
			result |= ImGuiWindowFlags_NoResize;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noMove))
			result |= ImGuiWindowFlags_NoMove;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noScrollbar))
			result |= ImGuiWindowFlags_NoScrollbar;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noScrollWithMouse))
			result |= ImGuiWindowFlags_NoScrollWithMouse;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noCollapse))
			result |= ImGuiWindowFlags_NoCollapse;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noBackground))
			result |= ImGuiWindowFlags_NoBackground;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noSavedSettings))
			result |= ImGuiWindowFlags_NoSavedSettings;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::menuBar))
			result |= ImGuiWindowFlags_MenuBar;
		if (emberCommon::HasFlag(flags, emberCommon::GuiWindowFlags::noDocking))
			result |= ImGuiWindowFlags_NoDocking;
		return result;
	}
	ImGuiFocusedFlags GuiFocusedFlagsCommonToImGui(emberCommon::GuiFocusedFlags flags)
	{
		ImGuiFocusedFlags result = 0;
		if (emberCommon::HasFlag(flags, emberCommon::GuiFocusedFlags::childWindows))
			result |= ImGuiFocusedFlags_ChildWindows;
		if (emberCommon::HasFlag(flags, emberCommon::GuiFocusedFlags::rootWindow))
			result |= ImGuiFocusedFlags_RootWindow;
		if (emberCommon::HasFlag(flags, emberCommon::GuiFocusedFlags::anyWindow))
			result |= ImGuiFocusedFlags_AnyWindow;
		return result;
	}
	ImGuiHoveredFlags GuiHoveredFlagsCommonToImGui(emberCommon::GuiHoveredFlags flags)
	{
		ImGuiHoveredFlags result = 0;
		if (emberCommon::HasFlag(flags, emberCommon::GuiHoveredFlags::childWindows))
			result |= ImGuiHoveredFlags_ChildWindows;
		if (emberCommon::HasFlag(flags, emberCommon::GuiHoveredFlags::rootWindow))
			result |= ImGuiHoveredFlags_RootWindow;
		if (emberCommon::HasFlag(flags, emberCommon::GuiHoveredFlags::anyWindow))
			result |= ImGuiHoveredFlags_AnyWindow;
		return result;
	}
	ImGuiButtonFlags GuiButtonFlagsCommonToImGui(emberCommon::GuiButtonFlags flags)
	{
		ImGuiButtonFlags result = 0;
		if (emberCommon::HasFlag(flags, emberCommon::GuiButtonFlags::mouseButtonLeft))
			result |= ImGuiButtonFlags_MouseButtonLeft;
		if (emberCommon::HasFlag(flags, emberCommon::GuiButtonFlags::mouseButtonRight))
			result |= ImGuiButtonFlags_MouseButtonRight;
		if (emberCommon::HasFlag(flags, emberCommon::GuiButtonFlags::mouseButtonMiddle))
			result |= ImGuiButtonFlags_MouseButtonMiddle;
		return result;
	}
	ImGuiInputTextFlags GuiInputTextFlagsCommonToImGui(emberCommon::GuiInputTextFlags flags)
	{
		ImGuiInputTextFlags result = 0;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::charsDecimal))
			result |= ImGuiInputTextFlags_CharsDecimal;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::charsHexadecimal))
			result |= ImGuiInputTextFlags_CharsHexadecimal;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::charsScientific))
			result |= ImGuiInputTextFlags_CharsScientific;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::charsUppercase))
			result |= ImGuiInputTextFlags_CharsUppercase;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::charsNoBlank))
			result |= ImGuiInputTextFlags_CharsNoBlank;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::allowTabInput))
			result |= ImGuiInputTextFlags_AllowTabInput;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::enterReturnsTrue))
			result |= ImGuiInputTextFlags_EnterReturnsTrue;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::escapeClearsAll))
			result |= ImGuiInputTextFlags_EscapeClearsAll;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::ctrlEnterForNewLine))
			result |= ImGuiInputTextFlags_CtrlEnterForNewLine;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::readOnly))
			result |= ImGuiInputTextFlags_ReadOnly;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::password))
			result |= ImGuiInputTextFlags_Password;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::alwaysOverwrite))
			result |= ImGuiInputTextFlags_AlwaysOverwrite;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::autoSelectAll))
			result |= ImGuiInputTextFlags_AutoSelectAll;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::noHorizontalScroll))
			result |= ImGuiInputTextFlags_NoHorizontalScroll;
		if (emberCommon::HasFlag(flags, emberCommon::GuiInputTextFlags::noUndoRedo))
			result |= ImGuiInputTextFlags_NoUndoRedo;
		return result;
	}
	ImGuiMouseButton GuiMouseButtonCommonToImGui(emberCommon::GuiMouseButton button)
	{
		switch (button)
		{
			case emberCommon::GuiMouseButton::left: return ImGuiMouseButton_Left;
			case emberCommon::GuiMouseButton::right: return ImGuiMouseButton_Right;
			case emberCommon::GuiMouseButton::middle: return ImGuiMouseButton_Middle;
			default: return ImGuiMouseButton_Left;
		}
	}
}