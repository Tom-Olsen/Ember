#pragma once
#include "commonGuiFlags.h"
#include <memory>
#include <string>
#include <unordered_set>



namespace emberEngine
{
	// Forward declerations:
	class EditorWindow;



	class Editor
	{
		// Friends:
		friend class EditorWindow;

	private: // Members:
		static EditorWindow* s_pFocusedWindow;
		static EditorWindow* s_pCurrentRenderedWindow;
		static std::unordered_set<EditorWindow*> s_pEditorWindows;

	public: // Methods:
		// Editor functionality:
		static void Render();

		// EditorWindow->editor communication:
		static void SetFocusedWindow(EditorWindow* pEditorWindow);
		static void AddEditorWindow(EditorWindow* pEditorWindow);
		static void DeleteEditorWindow(EditorWindow* pEditorWindow);

		// Getters:
		static EditorWindow* GetFocusedWindow();
		static bool GetFocusedWindowWantCaptureEvents();
		static float GetSpacingX();
		static float GetWindowWidth();
		static float GetWindowHeight();
		static float GetRemainingWidth();
		static const std::string& GetWindowName();
		static int GetWindowID();
		static const std::string& GetWindowNameID();

	private: // Methods:
		// Delete all constructors:
		Editor() = delete;
		Editor(const Editor&) = delete;
		Editor& operator=(const Editor&) = delete;
		Editor(Editor&&) = delete;
		Editor& operator=(Editor&&) = delete;
		~Editor() = delete;
	};
}