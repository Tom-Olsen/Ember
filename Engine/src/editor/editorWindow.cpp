#include "editorWindow.h"
#include "editor.h"
#include "logger.h"
#include <imgui.h>



namespace emberEngine
{
	// Constructor/Destructor:
	EditorWindow::EditorWindow()
	{
		Editor::AddEditorWindow(this);
	}
	EditorWindow::~EditorWindow()
	{
		Editor::DeleteEditorWindow(this);
	}

	void EditorWindow::Render() const
	{
		LOG_WARN("EditorWindow::Render override missing.");
	}
}