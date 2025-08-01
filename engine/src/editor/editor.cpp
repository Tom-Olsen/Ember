#include "editor.h"
#include "editorWindow.h"
#include "logger.h"
#include "macros.h"



namespace emberEngine
{
	// Static members:
	float Editor::s_intDragSensitivityBase = 0.1f;
	float Editor::s_floatDragSensitivityBase = 0.01f;
	float Editor::s_dragSensitivityExponent = 1.5f;
	float Editor::s_minWidgetWidth = 50.0f;
	float Editor::s_labelPercentile = 0.4f;
	std::unordered_map<std::string, float> Editor::s_intDragAccumulators;
	std::unordered_set<std::string> Editor::s_validIntDragAccumulators;
	std::unordered_set<EditorWindow*> Editor::s_editorWindows;
	EditorWindow* Editor::s_pFocusedWindow = nullptr;
	EditorWindow* Editor::s_pCurrentRenderedWindow = nullptr;



	// Initialization and cleanup:
	void Editor::Init()
	{
		#ifdef LOG_INITIALIZATION
		LOG_TRACE("Editor initialized.");
		#endif
	}
	void Editor::Clear()
	{
		s_intDragAccumulators.clear();
		s_validIntDragAccumulators.clear();
		s_editorWindows.clear();
	}



	// Basic functionality:
	void Editor::Render()
	{
		// Invalidate window pointers:
		s_pFocusedWindow = nullptr;
		s_pCurrentRenderedWindow = nullptr;

		for (EditorWindow* editorWindow : s_editorWindows)
		{
			if (editorWindow == nullptr)
				continue;
			s_pCurrentRenderedWindow = editorWindow;
			editorWindow->BaseRender();
		}

		// Clear invalid int drag accumulators:
		for (auto it = s_intDragAccumulators.begin(); it != s_intDragAccumulators.end();)
		{
			if (s_validIntDragAccumulators.find(it->first) == s_validIntDragAccumulators.end())
				it = s_intDragAccumulators.erase(it);
			else
				++it;
		}
		s_validIntDragAccumulators.clear();
	}
	EditorWindow* Editor::GetFocusedWindow()
	{
		return s_pFocusedWindow;
	}
	void Editor::SetFocusedWindow(EditorWindow* pEditorWindow)
	{
		s_pFocusedWindow = pEditorWindow;
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
	bool Editor::Checkbox(const std::string& label, bool* v)
	{
		// Draw label:
		ImGui::TextUnformatted(label.c_str());

		// Move cursor to label|fields seperator:
		ImGui::SameLine();
		ImVec2 cursorPos = ImGui::GetCursorPos();
		cursorPos.x = std::max(cursorPos.x, s_labelPercentile * GetWindowWidth());
		ImGui::SetCursorPos(cursorPos);

		bool changed = false;
		ImGui::PushID(label.c_str());
		{
			// Clamp input width:
			float inputWidth = GetRemainingWidth() - GetSpacingX();
			inputWidth = std::max(inputWidth, s_minWidgetWidth);

			// Draw input field:
			ImGui::SetNextItemWidth(inputWidth);
			changed = ImGui::Checkbox(("##" + label).c_str(), v);
		}
		ImGui::PopID();
		return changed;
	}
	void Editor::Text(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
	}
	void Editor::TextUnformatted(const char* text)
	{
		ImGui::TextUnformatted(text);
	}
	bool Editor::InputInt(const std::string& label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
	{
		// Draw label:
		ImGui::TextUnformatted(label.c_str());

		// Move cursor to label|fields seperator:
		ImGui::SameLine();
		ImVec2 cursorPos = ImGui::GetCursorPos();
		cursorPos.x = std::max(cursorPos.x, s_labelPercentile * GetWindowWidth());
		ImGui::SetCursorPos(cursorPos);

		bool changed = false;
		ImGui::PushID(label.c_str());
		{
			// Clamp input width:
			float inputWidth = GetRemainingWidth() - GetSpacingX();
			inputWidth = std::max(inputWidth, s_minWidgetWidth);

			// Draw input field:
			ImGui::SetNextItemWidth(inputWidth);
			changed = ImGui::InputInt("##Input", v, step, step_fast, flags);
		}
		ImGui::PopID();
		return changed;
	}
	bool Editor::DragInt(const std::string& label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
	{
		// Accumulation:
		std::string id = GetWindowNameID() + label;
		float accumulator = s_intDragAccumulators[id];
		s_validIntDragAccumulators.emplace(id);

		bool changed = false;
		ImGui::PushID(label.c_str());
		{
			ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();

			// Invisible interactive area for click and drag detection:
			bool mouseUp = ImGui::InvisibleButton("##drag_text", textSize);
			bool mouseDown = ImGui::IsItemActivated();
			bool dragging = ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left);

			// Draw text above invisible button:
			ImGui::SetCursorScreenPos(cursorPos);
			ImGui::TextUnformatted(label.c_str());

			// On initial interaction get int value:
			if (mouseDown)
				accumulator = *v;

			// Change current value on dragging motion on label:
			if (dragging)
			{
				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

				float sensitivity = s_intDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
				float sign = (delta.x >= 0.0f) ? 1.0f : -1.0f;
				accumulator += sign * sensitivity;
				*v = static_cast<int>(std::round(accumulator));

				ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			}

			// Move cursor to label|fields seperator:
			ImGui::SameLine();
			cursorPos = ImGui::GetCursorPos();
			cursorPos.x = std::max(cursorPos.x, s_labelPercentile * GetWindowWidth());
			ImGui::SetCursorPos(cursorPos);

			// Float input field:
			float inputWidth = GetRemainingWidth() - GetSpacingX();
			inputWidth = std::max(inputWidth, s_minWidgetWidth);
			ImGui::SetNextItemWidth(inputWidth);
			bool interaction = ImGui::InputInt("##Input", v, step, step_fast, flags);

			s_intDragAccumulators[id] = accumulator;
			changed = dragging || interaction;
		}
		ImGui::PopID();
		return changed;
	}
	bool Editor::InputFloat(const std::string& label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
	{
		// Draw label:
		ImGui::TextUnformatted(label.c_str());

		// Move cursor to label|fields seperator:
		ImGui::SameLine();
		ImVec2 cursorPos = ImGui::GetCursorPos();
		cursorPos.x = std::max(cursorPos.x, s_labelPercentile * GetWindowWidth());
		ImGui::SetCursorPos(cursorPos);

		bool changed = false;
		ImGui::PushID(label.c_str());
		{
			// Clamp input width:
			float inputWidth = GetRemainingWidth() - GetSpacingX();
			inputWidth = std::max(inputWidth, s_minWidgetWidth);

			// Draw input field:
			ImGui::SetNextItemWidth(inputWidth);
			changed = ImGui::InputFloat("##Input", v, step, step_fast, format, flags);
		}
		ImGui::PopID();
		return changed;
	}
	bool Editor::DragFloat(const std::string& label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());
		{
			ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();

			// Invisible interactive area for click and drag detection:
			bool clicked = ImGui::InvisibleButton("##drag_text", textSize);
			bool dragging = ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left);

			// Draw text above invisible button:
			ImGui::SetCursorScreenPos(cursorPos);
			ImGui::TextUnformatted(label.c_str());

			// Change current value on dragging motion on label:
			if (dragging)
			{
				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

				float sensitivity = s_floatDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
				float sign = (delta.x >= 0.0f) ? 1.0f : -1.0f;
				*v += sign * sensitivity;

				ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			}

			// Move cursor to label|fields seperator:
			ImGui::SameLine();
			cursorPos = ImGui::GetCursorPos();
			cursorPos.x = std::max(cursorPos.x, s_labelPercentile * GetWindowWidth());
			ImGui::SetCursorPos(cursorPos);	

			// Float input field:
			float inputWidth = GetRemainingWidth() - GetSpacingX();
			inputWidth = std::max(inputWidth, s_minWidgetWidth);
			ImGui::SetNextItemWidth(inputWidth);
			bool interaction = ImGui::InputFloat("##Input", v, step, step_fast, format, flags);

			changed = dragging || interaction;
		}
		ImGui::PopID();
		return changed;
	}



	// Private methods:
	float Editor::GetSpacingX()
	{
		return ImGui::GetStyle().ItemSpacing.x;
	}
	float Editor::GetWindowWidth()
	{
		return s_pCurrentRenderedWindow->GetWidth();
	}
	float Editor::GetWindowHeight()
	{
		return s_pCurrentRenderedWindow->GetHeight();
	}
	float Editor::GetRemainingWidth()
	{
		return GetWindowWidth() - ImGui::GetCursorPosX();
	}
	const std::string& Editor::GetWindowName()
	{
		return s_pCurrentRenderedWindow->GetName();
	}
	int Editor::GetWindowID()
	{
		return s_pCurrentRenderedWindow->GetID();
	}
	const std::string& Editor::GetWindowNameID()
	{
		return s_pCurrentRenderedWindow->GetNameID();
	}
}