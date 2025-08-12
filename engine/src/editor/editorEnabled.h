#ifndef __INCLUDE_GUARD_editorEnabled_h__
#define __INCLUDE_GUARD_editorEnabled_h__
#include "dearImGuiFlags.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <imgui.h>



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
	private: // Members:
		static float s_intDragSensitivityBase;
		static float s_floatDragSensitivityBase;
		static float s_dragSensitivityExponent;
		static float s_minWidgetWidth;
		static float s_labelPercentile;
		static std::unordered_set<EditorWindow*> s_editorWindows;
		static EditorWindow* s_pFocusedWindow;
		static EditorWindow* s_pCurrentRenderedWindow;

		// Editor windows:
		static std::unique_ptr<ConsoleEditorWindow> s_pConsoleEditorWindow;
		static std::unique_ptr<DepthBiasEditorWindow> s_pDepthBiasEditorWindow;
		static std::unique_ptr<FpsEditorWindow> s_pFpsEditorWindow;
		static std::unique_ptr<GameEditorWindow> s_pGameEditorWindow;
		static std::unique_ptr<HierarchyEditorWindow> s_pHierarchyEditorWindow;
		static std::unique_ptr<InspectorEditorWindow> s_pInspectorEditorWindow;
		static std::unique_ptr<ProjectEditorWindow> s_pProjectEditorWindow;
		static std::unique_ptr<SceneEditorWindow> s_pSceneEditorWindow;

	public: // Methods:
		static void Init(bool renderToImGuiWindow);
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
		static bool InputInt(const std::string& label, int* v, int step = 1, int step_fast = 100, WindowFlags flags = 0);
		static bool DragInt(const std::string& label, int* v, int step = 1, int step_fast = 100, WindowFlags flags = 0);
		static bool InputFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", WindowFlags flags = 0);
		static bool DragFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", WindowFlags flags = 0);

		// Setters:
		static void SetScene(Scene* pScene);

		// Getters:
		ConsoleEditorWindow* GetConsoleEditorWindow() const;
		DepthBiasEditorWindow* GetDepthBiasEditorWindow() const;
		FpsEditorWindow* GetFpsEditorWindow() const;
		GameEditorWindow* GetGameEditorWindow() const;
		HierarchyEditorWindow* GetHierarchyEditorWindow() const;
		InspectorEditorWindow* GetInspectorEditorWindow() const;
		ProjectEditorWindow* GetProjectEditorWindow() const;
		SceneEditorWindow* GetSceneEditorWindow() const;

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



#endif // __INCLUDE_GUARD_editorEnabled_h__