#include "imGuiConvertGuiStyle.h"



namespace imGuiSdlVulkanBackend
{
	emberCommon::GuiStyle GuiStyleImGuiToCommon(const ImGuiStyle& style)
	{
		emberCommon::GuiStyle commonStyle;
		commonStyle.fontSize = style.FontSizeBase;
		commonStyle.alpha = style.Alpha;
		commonStyle.windowPadding = Float2{ style.WindowPadding.x, style.WindowPadding.y };
		commonStyle.framePadding = Float2{ style.FramePadding.x, style.FramePadding.y };
		commonStyle.itemSpacing = Float2{ style.ItemSpacing.x, style.ItemSpacing.y };
		commonStyle.itemInnerSpacing = Float2{ style.ItemInnerSpacing.x, style.ItemInnerSpacing.y };
		commonStyle.cellPadding = Float2{ style.CellPadding.x, style.CellPadding.y };
		commonStyle.indentSpacing = style.IndentSpacing;
		commonStyle.scrollbarSize = style.ScrollbarSize;
		commonStyle.grabMinSize = style.GrabMinSize;
		commonStyle.windowRounding = style.WindowRounding;
		commonStyle.frameRounding = style.FrameRounding;
		commonStyle.windowTitleTextAlign = Float2{ style.WindowTitleAlign.x, style.WindowTitleAlign.y };
		commonStyle.buttonTextAlign = Float2{ style.ButtonTextAlign.x, style.ButtonTextAlign.y };
		commonStyle.selectableTextAlign = Float2{ style.SelectableTextAlign.x, style.SelectableTextAlign.y };
		commonStyle.separatorTextAlign = Float2{ style.SeparatorTextAlign.x, style.SeparatorTextAlign.y };
		commonStyle.separatorTextPadding = Float2{ style.SeparatorTextPadding.x, style.SeparatorTextPadding.y };
		commonStyle.tableAngledHeaderTextAlign = Float2{ style.TableAngledHeadersTextAlign.x, style.TableAngledHeadersTextAlign.y };
		commonStyle.selectedTabCloseButtonMinWidth = style.TabCloseButtonMinWidthSelected;
		commonStyle.unselectedTabCloseButtonMinWidth = style.TabCloseButtonMinWidthUnselected;
		return commonStyle;
	}
	void ApplyGuiStyleCommonToImGui(const emberCommon::GuiStyle& commonStyle, ImGuiStyle& imGuiStyle)
	{
		imGuiStyle.FontSizeBase = commonStyle.fontSize;
		imGuiStyle.Alpha = commonStyle.alpha;
		imGuiStyle.WindowPadding = ImVec2{ commonStyle.windowPadding.x, commonStyle.windowPadding.y };
		imGuiStyle.FramePadding = ImVec2{ commonStyle.framePadding.x, commonStyle.framePadding.y };
		imGuiStyle.ItemSpacing = ImVec2{ commonStyle.itemSpacing.x, commonStyle.itemSpacing.y };
		imGuiStyle.ItemInnerSpacing = ImVec2{ commonStyle.itemInnerSpacing.x, commonStyle.itemInnerSpacing.y };
		imGuiStyle.CellPadding = ImVec2{ commonStyle.cellPadding.x, commonStyle.cellPadding.y };
		imGuiStyle.IndentSpacing = commonStyle.indentSpacing;
		imGuiStyle.ScrollbarSize = commonStyle.scrollbarSize;
		imGuiStyle.GrabMinSize = commonStyle.grabMinSize;
		imGuiStyle.WindowRounding = commonStyle.windowRounding;
		imGuiStyle.FrameRounding = commonStyle.frameRounding;
		imGuiStyle.WindowTitleAlign = ImVec2{ commonStyle.windowTitleTextAlign.x, commonStyle.windowTitleTextAlign.y };
		imGuiStyle.ButtonTextAlign = ImVec2{ commonStyle.buttonTextAlign.x, commonStyle.buttonTextAlign.y };
		imGuiStyle.SelectableTextAlign = ImVec2{ commonStyle.selectableTextAlign.x, commonStyle.selectableTextAlign.y };
		imGuiStyle.SeparatorTextAlign = ImVec2{ commonStyle.separatorTextAlign.x, commonStyle.separatorTextAlign.y };
		imGuiStyle.SeparatorTextPadding = ImVec2{ commonStyle.separatorTextPadding.x, commonStyle.separatorTextPadding.y };
		imGuiStyle.TableAngledHeadersTextAlign = ImVec2{ commonStyle.tableAngledHeaderTextAlign.x, commonStyle.tableAngledHeaderTextAlign.y };
		imGuiStyle.TabCloseButtonMinWidthSelected = commonStyle.selectedTabCloseButtonMinWidth;
		imGuiStyle.TabCloseButtonMinWidthUnselected = commonStyle.unselectedTabCloseButtonMinWidth;
	}
}