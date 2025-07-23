#ifndef __INCLUDE_GUARD_editor_h__
#define __INCLUDE_GUARD_editor_h__
#include <memory>
#include <string>
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
		static std::unordered_set<EditorWindow*> s_editorWindows;

	public: // Methods:
		static void Init();
		static void Clear();

		// Basic functionality:
		static void Render();
		static void AddEditorWindow(EditorWindow* pEditorWindow);
		static void DeleteEditorWindow(EditorWindow* pEditorWindow);

		// Editor functions:
		static void Checkbox(const std::string& label, bool* v);
		static void Text(const char* fmt, ...);
		static bool InputInt(const std::string& label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
		static bool InputFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	};
}



#endif // __INCLUDE_GUARD_editor_h__