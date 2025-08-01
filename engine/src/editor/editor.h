#ifndef __INCLUDE_GUARD_editor_h__
#define __INCLUDE_GUARD_editor_h__
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <imgui.h>



namespace emberEngine
{
	// Forward declerations:
	class EditorWindow;



	/// <summary>
	/// Static class that manages all editor windows and hands them over to dearImGui for rendering.
	/// </summary>
	class Editor
	{
	private: // Members:
		static float s_intDragSensitivityBase;
		static float s_floatDragSensitivityBase;
		static float s_dragSensitivityExponent;
		static float s_minWidgetWidth;
		static float s_labelPercentile;
		static std::unordered_map<std::string, float> s_intDragAccumulators;
		static std::unordered_set<std::string> s_validIntDragAccumulators;
		static std::unordered_set<EditorWindow*> s_editorWindows;
		static EditorWindow* s_pFocusedWindow;
		static EditorWindow* s_pCurrentRenderedWindow;

	public: // Methods:
		static void Init();
		static void Clear();

		// Basic functionality:
		static void Render();
		static EditorWindow* GetFocusedWindow();
		static void SetFocusedWindow(EditorWindow* pEditorWindow);
		static void AddEditorWindow(EditorWindow* pEditorWindow);
		static void DeleteEditorWindow(EditorWindow* pEditorWindow);

		// Editor functions:
		static bool Checkbox(const std::string& label, bool* v);
		static void Text(const char* fmt, ...);
		static void TextUnformatted(const char* text);
		static bool InputInt(const std::string& label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
		static bool DragInt(const std::string& label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
		static bool InputFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
		static bool DragFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

	private: // Methods:
		static float GetSpacingX();
		static float GetWindowWidth();
		static float GetWindowHeight();
		static float GetRemainingWidth();
		static const std::string& GetWindowName();
		static int GetWindowID();
		static const std::string& GetWindowNameID();

		// Delete all constructors.
		Editor() = delete;
		Editor(const Editor&) = delete;
		Editor& operator=(const Editor&) = delete;
		Editor(Editor&&) = delete;
		Editor& operator=(Editor&&) = delete;
		~Editor() = delete;
	};
}



#endif // __INCLUDE_GUARD_editor_h__