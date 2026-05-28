#pragma once
#include "commonGuiFlags.h"
#include <imgui.h>



namespace imGuiSdlVulkanBackend
{
	[[nodiscard]] ImGuiWindowFlags GuiWindowFlagsCommonToImGui(emberCommon::GuiWindowFlags flags);
	[[nodiscard]] ImGuiFocusedFlags GuiFocusedFlagsCommonToImGui(emberCommon::GuiFocusedFlags flags);
	[[nodiscard]] ImGuiHoveredFlags GuiHoveredFlagsCommonToImGui(emberCommon::GuiHoveredFlags flags);
	[[nodiscard]] ImGuiButtonFlags GuiButtonFlagsCommonToImGui(emberCommon::GuiButtonFlags flags);
	[[nodiscard]] ImGuiInputTextFlags GuiInputTextFlagsCommonToImGui(emberCommon::GuiInputTextFlags flags);
	[[nodiscard]] ImGuiMouseButton GuiMouseButtonCommonToImGui(emberCommon::GuiMouseButton button);
}