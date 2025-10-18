#include "editorWindow.h"
#include "editor.h"
#include "gui.h"
#include "logger.h"



namespace emberEngine
{
	// Constructors/Destructor:
	EditorWindow::EditorWindow()
	{
		Editor::AddEditorWindow(this);
	}
	EditorWindow::~EditorWindow()
	{
		Editor::DeleteEditorWindow(this);
	}



	// Movable:
	EditorWindow::EditorWindow(EditorWindow&& other) noexcept = default;
	EditorWindow& EditorWindow::operator=(EditorWindow&& other) noexcept = default;



	// Rendering:
	void EditorWindow::Open()
	{
		m_isOpen = true;
	}
	void EditorWindow::RenderWindow()
	{
		// Skip rendering if window is closed:
		if (!m_isOpen)
			return;

		Gui::PushID(m_nameID.c_str());
		if (Gui::Begin(m_nameID.c_str(), &m_isOpen, m_windowFlags))
		{
			// Get window size:
			Float2 size = Gui::GetWindowSize();
			m_width = size.x;
			m_height = size.y;

			// Call the overridden Render() method of the derived class:
			Render();

			// Let Editor know if this window is focused:
			m_isFocused = Gui::IsWindowFocused(emberCommon::GuiFocusedFlag::GuiFocusedFlag_RootAndChildWindows);
			if (m_isFocused)
				Editor::SetFocusedWindow(this);
		}
		Gui::End();	// must be called even if ImGui::Begin() returns false.
		Gui::PopID();
	}
	void EditorWindow::Render()
	{
		LOG_WARN("EditorWindow::Render override missing.");
	}



	// Getters:
	const std::string& EditorWindow::GetName() const
	{
		return m_name;
	}
	int EditorWindow::GetID() const
	{
		return m_ID;
	}
	const std::string& EditorWindow::GetNameID() const
	{
		return m_nameID;
	}
	float EditorWindow::GetWidth() const
	{
		return m_width;
	}
	float EditorWindow::GetHeight() const
	{
		return m_height;
	}
	bool EditorWindow::WantCaptureEvents() const
	{
		return m_wantCaptureEvents;
	}
}