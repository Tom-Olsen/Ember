#ifndef __INCLUDE_GUARD_editorDisabled_h__
#define __INCLUDE_GUARD_editorDisabled_h__
#include <string>



namespace emberEngine
{
	// Forward declerations:
	class Scene;
	class ConsoleEditorWindow;
	class DepthBiasEditorWindow;
	class EditorWindow;
	class FpsEditorWindow;
	class GameEditorWindow;
	class HierarchyEditorWindow;
	class InspectorEditorWindow;
	class ProjectEditorWindow;
	class SceneEditorWindow;

	// Ember::ToDo: fix windowWidth when scrollbar is visable.

	/// <summary>
	/// Static class that manages all editor windows and hands them over to dearImGui for rendering.
	/// </summary>
	class Editor
	{
	public: // Methods:
		inline static void Init(bool renderToImGuiWindow) {}
		inline static void Clear() {}

		// Basic functionality:
		inline static void Render() {}
		inline static EditorWindow* GetFocusedWindow() {}
		inline static void SetFocusedWindow(EditorWindow* pEditorWindow) {}
		inline static void AddEditorWindow(EditorWindow* pEditorWindow) {}
		inline static void DeleteEditorWindow(EditorWindow* pEditorWindow) {}

		// Editor functions:
		inline static bool Checkbox(const std::string& label, bool* v) { return false; }
		inline static void Text(const char* fmt, ...) {}
		inline static void TextUnformatted(const char* text) {}
		inline static bool InputInt(const std::string& label, int* v, int step = 1, int step_fast = 100, int flags = 0) { return false; }
		inline static bool DragInt(const std::string& label, int* v, int step = 1, int step_fast = 100, int flags = 0) { return false; }
		inline static bool InputFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", int flags = 0) { return false; }
		inline static bool DragFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", int flags = 0) { return false; }

		// Setters:
		inline static void SetScene(Scene* pScene) {}

		// Getters:
		inline ConsoleEditorWindow* GetConsoleEditorWindow() const { return nullptr; }
		inline DepthBiasEditorWindow* GetDepthBiasEditorWindow() const { return nullptr; }
		inline FpsEditorWindow* GetFpsEditorWindow() const { return nullptr; }
		inline GameEditorWindow* GetGameEditorWindow() const { return nullptr; }
		inline HierarchyEditorWindow* GetHierarchyEditorWindow() const { return nullptr; }
		inline InspectorEditorWindow* GetInspectorEditorWindow() const { return nullptr; }
		inline ProjectEditorWindow* GetProjectEditorWindow() const { return nullptr; }
		inline SceneEditorWindow* GetSceneEditorWindow() const { return nullptr; }
	};
}



#endif // __INCLUDE_GUARD_editorDisabled_h__