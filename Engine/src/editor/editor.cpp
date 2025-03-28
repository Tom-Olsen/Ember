#include "editor.h"
#include "editorWindow.h"
#include "logger.h"



namespace emberEngine
{
	// Static members:
	std::unordered_set<EditorWindow*> Editor::s_editorWindows;



	// Initialization and cleanup:
	void Editor::Init()
	{

	}
	void Editor::Clear()
	{
		s_editorWindows.clear();
	}



	// Render all editorWindows:
	void Editor::Render()
	{
		for (EditorWindow* editorWindow : s_editorWindows)
			editorWindow->Render();
	}

	// EditorWindow management:
	void Editor::AddEditorWindow(EditorWindow* pEditorWindow)
	{
		s_editorWindows.emplace(pEditorWindow);
	}
	void Editor::DeleteEditorWindow(EditorWindow* pEditorWindow)
	{
		s_editorWindows.erase(pEditorWindow);
	}
}