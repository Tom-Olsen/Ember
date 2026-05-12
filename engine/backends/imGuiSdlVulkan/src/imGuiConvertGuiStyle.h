#pragma once
#include "commonGuiStyle.h"
#include <imgui.h>



namespace imGuiSdlVulkanBackend
{
	[[nodiscard]] emberCommon::GuiStyle GuiStyleImGuiToCommon(const ImGuiStyle& style);
	void ApplyGuiStyleCommonToImGui(const emberCommon::GuiStyle& commonStyle, ImGuiStyle& imGuiStyle);
}