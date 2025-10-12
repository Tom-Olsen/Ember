#pragma once
#include "commonGuiFlags.h"
#include "emberEditorExport.h"
#include "iEditor.h"
#include <memory>
#include <string>
#include <unordered_set>

void EMBER_EDITOR_API Foo();

//namespace emberEditorBackend
//{
//	// Forward declerations:
//	class Scene;
//	class ConsoleEditorWindow;
//	class DepthBiasEditorWindow;
//	class EditorWindow;
//	class FpsEditorWindow;
//	class GameEditorWindow;
//	class HierarchyEditorWindow;
//	class InspectorEditorWindow;
//	class ProjectEditorWindow;
//	class SceneEditorWindow;
//
//	// Ember::ToDo: fix windowWidth when scrollbar is visable.
//
//	class EMBER_EDITOR_API Editor : public emberBackendInterface::IEditor
//	{
//	private: // Members:
//		float s_intDragSensitivityBase;
//		float s_floatDragSensitivityBase;
//		float s_dragSensitivityExponent;
//		float s_minWidgetWidth;
//		float s_labelPercentile;
//		std::unordered_set<EditorWindow*> s_editorWindows;
//		EditorWindow* s_pFocusedWindow;
//		EditorWindow* s_pCurrentRenderedWindow;
//
//		// Editor windows:
//		std::unique_ptr<ConsoleEditorWindow> s_pConsoleEditorWindow;
//		std::unique_ptr<DepthBiasEditorWindow> s_pDepthBiasEditorWindow;
//		std::unique_ptr<FpsEditorWindow> s_pFpsEditorWindow;
//		std::unique_ptr<GameEditorWindow> s_pGameEditorWindow;
//		std::unique_ptr<HierarchyEditorWindow> s_pHierarchyEditorWindow;
//		std::unique_ptr<InspectorEditorWindow> s_pInspectorEditorWindow;
//		std::unique_ptr<ProjectEditorWindow> s_pProjectEditorWindow;
//		std::unique_ptr<SceneEditorWindow> s_pSceneEditorWindow;
//
//	public: // Methods:
//		// Constructor/Destructor:
//		Editor();
//		~Editor();
//
//		// Non-copyable:
//		Editor(const Editor&) = delete;
//		Editor& operator=(const Editor&) = delete;
//
//		// Movable:
//		Editor(Editor&& other) noexcept;
//		Editor& operator=(Editor&& other) noexcept;
//
//		// Interface overrides:
//		void Render() override;
//		EditorWindow* GetFocusedWindow() override;
//		bool GetWindowWantCaptureEvents() override;
//
//		void SetFocusedWindow(EditorWindow* pEditorWindow);
//		void AddEditorWindow(EditorWindow* pEditorWindow);
//		void DeleteEditorWindow(EditorWindow* pEditorWindow);
//
//		// Editor functions:
//		bool Checkbox(const std::string& label, bool* v);
//		void Text(const char* fmt, ...);
//		void TextUnformatted(const char* text);
//		bool InputInt(const std::string& label, int* v, int step = 1, int step_fast = 100, emberCommon::GuiWindowFlags flags = 0);
//		bool DragInt(const std::string& label, int* v, int step = 1, int step_fast = 100, emberCommon::GuiWindowFlags flags = 0);
//		bool InputFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", emberCommon::GuiWindowFlags flags = 0);
//		bool DragFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", emberCommon::GuiWindowFlags flags = 0);
//
//		// Setters:
//		void SetScene(Scene* pScene);
//
//		// Getters:
//		ConsoleEditorWindow* GetConsoleEditorWindow() const;
//		DepthBiasEditorWindow* GetDepthBiasEditorWindow() const;
//		FpsEditorWindow* GetFpsEditorWindow() const;
//		GameEditorWindow* GetGameEditorWindow() const;
//		HierarchyEditorWindow* GetHierarchyEditorWindow() const;
//		InspectorEditorWindow* GetInspectorEditorWindow() const;
//		ProjectEditorWindow* GetProjectEditorWindow() const;
//		SceneEditorWindow* GetSceneEditorWindow() const;
//
//	private: // Methods:
//		float GetSpacingX();
//		float GetWindowWidth();
//		float GetWindowHeight();
//		float GetRemainingWidth();
//		const std::string& GetWindowName();
//		int GetWindowID();
//		const std::string& GetWindowNameID();
//	};
//}