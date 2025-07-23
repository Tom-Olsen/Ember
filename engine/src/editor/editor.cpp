#include "editor.h"
#include "editorWindow.h"
#include "logger.h"
#include "macros.h"



namespace emberEngine
{
	// Static members:
	std::unordered_set<EditorWindow*> Editor::s_editorWindows;



	// Initialization and cleanup:
	void Editor::Init()
	{
		#ifdef LOG_INITIALIZATION
		LOG_TRACE("Editor initialized.");
		#endif
	}
	void Editor::Clear()
	{
		s_editorWindows.clear();
	}



	// Basic functionality:
	void Editor::Render()
	{
		for (EditorWindow* editorWindow : s_editorWindows)
			editorWindow->Render();
	}
	void Editor::AddEditorWindow(EditorWindow* pEditorWindow)
	{
		s_editorWindows.emplace(pEditorWindow);
	}
	void Editor::DeleteEditorWindow(EditorWindow* pEditorWindow)
	{
		s_editorWindows.erase(pEditorWindow);
	}

	// Editor functions:
	void Editor::Checkbox(const std::string& label, bool* v)
	{
		// Left aligned:
		//ImGui::Text(label.c_str());
		//ImGui::SameLine();
		//ImGui::Checkbox(("##" + label).c_str(), v);

		// Right aligned:
		// Calculate the width of the label and checkbox
		float labelWidth = ImGui::CalcTextSize(label.c_str()).x;
		float checkboxWidth = ImGui::GetTextLineHeight();  // Approximate size of the checkbox
		float border = 10;

		// Get available space to the right
		float availableSpace = ImGui::GetContentRegionAvail().x;

		// Set cursor position to align right
		ImGui::SetCursorPosX(availableSpace - labelWidth - checkboxWidth - border);

		// Render the label and checkbox
		ImGui::Text("%s", label.c_str());
		ImGui::SameLine();
		ImGui::Checkbox(("##" + label).c_str(), v);
	}
	void Editor::Text(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
	}
	bool Editor::InputInt(const std::string& label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
	{
		ImGui::Text("%s", label.c_str());
		ImGui::SameLine();
		return ImGui::InputInt(("##" + label).c_str(), v, step, step_fast, flags);
	}
	bool Editor::InputFloat(const std::string& label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
	{
		ImGui::Text("%s", label.c_str());
		ImGui::SameLine();
		return ImGui::InputFloat(("##" + label).c_str(), v, step, step_fast, format, flags);
	}
}