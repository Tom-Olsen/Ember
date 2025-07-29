#include "editorWindow.h"
#include "editor.h"
#include "logger.h"



namespace emberEngine
{
	// Constructor/Destructor:
	EditorWindow::EditorWindow()
	{
		m_windowFlags = ImGuiWindowFlags_None;
		Editor::AddEditorWindow(this);
	}
	EditorWindow::~EditorWindow()
	{
		Editor::DeleteEditorWindow(this);
	}

	void EditorWindow::BaseRender()
	{
		if (!m_isOpen)
			return; // skip rendering if window is closed.

		if (ImGui::Begin(m_name.c_str(), &m_isOpen, m_windowFlags))
		{
			Render();
			m_isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
			if (m_isFocused)
				Editor::SetFocusedWindow(this);
		}
		ImGui::End();	// must be called even if ImGui::Begin() returns false.
	}
	void EditorWindow::Render()
	{
		LOG_WARN("EditorWindow::Render override missing.");
	}

	std::string EditorWindow::GetName() const
	{
		return m_name;
	}
	bool EditorWindow::WantCaptureEvents() const
	{
		return m_wantCaptureEvents;
	}
	void EditorWindow::Open()
	{
		m_isOpen = true;
	}
}