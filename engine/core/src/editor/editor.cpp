#include "editor.h"
#include "editorWindow.h"
#include "gui.h"



namespace emberEngine
{
	// Static members:
	EditorWindow* Editor::s_pFocusedWindow = nullptr;
	EditorWindow* Editor::s_pCurrentRenderedWindow = nullptr;
	std::unordered_set<EditorWindow*> Editor::s_pEditorWindows;



	// Public methods:
	// Editor functionality:
	void Editor::Render()
	{
		// Invalidate window pointers:
		s_pFocusedWindow = nullptr;
		s_pCurrentRenderedWindow = nullptr;

		for (EditorWindow* pEditorWindow : s_pEditorWindows)
		{
			if (pEditorWindow == nullptr)
				continue;
			s_pCurrentRenderedWindow = pEditorWindow;
			pEditorWindow->RenderWindow();
		}
	}



	// EditorWindow->editor communication:
	void Editor::SetFocusedWindow(EditorWindow* pEditorWindow)
	{
		s_pFocusedWindow = pEditorWindow;
	}
	void Editor::AddEditorWindow(EditorWindow* pEditorWindow)
	{
		s_pEditorWindows.emplace(pEditorWindow);
	}
	void Editor::DeleteEditorWindow(EditorWindow* pEditorWindow)
	{
		s_pEditorWindows.erase(pEditorWindow);
	}



	// Getters:
	EditorWindow* Editor::GetFocusedWindow()
	{
		return s_pFocusedWindow;
	}
	bool Editor::GetFocusedWindowWantCaptureEvents()
	{
		if (s_pFocusedWindow == nullptr)
			return false;
		return s_pFocusedWindow->WantCaptureEvents();
	}
	float Editor::GetSpacingX()
	{
		return Gui::GetStyle().ItemSpacing.x;
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
		return GetWindowWidth() - Gui::GetCursorPos().x;
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