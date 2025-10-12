//#include "editorWindow.h"
//#include "dearImGui.h"
//#include "editor.h"
//#include "logger.h"
//
//
//
//namespace emberEngine
//{
//	// Constructor/Destructor:
//	EditorWindow::EditorWindow()
//	{
//		Editor::AddEditorWindow(this);
//	}
//	EditorWindow::~EditorWindow()
//	{
//		Editor::DeleteEditorWindow(this);
//	}
//
//	void EditorWindow::BaseRender()
//	{
//		// Skip rendering if window is closed:
//		if (!m_isOpen)
//			return;
//
//		DearImGui::PushID(m_nameID.c_str());
//		if (DearImGui::Begin(m_nameID.c_str(), &m_isOpen, m_windowFlags))
//		{
//			// Get window size:
//			Float2 size = DearImGui::GetWindowSize();
//			m_width = size.x;
//			m_height = size.y;
//
//			// Call the overridden Render() method of the derived class:
//			Render();
//
//			// Let Editor know if this window is focused:
//			m_isFocused = DearImGui::IsWindowFocused(DearImGuiFocusedFlag_RootAndChildWindows);
//			if (m_isFocused)
//				Editor::SetFocusedWindow(this);
//		}
//		DearImGui::End();	// must be called even if ImGui::Begin() returns false.
//		DearImGui::PopID();
//	}
//	void EditorWindow::Render()
//	{
//		LOG_WARN("EditorWindow::Render override missing.");
//	}
//
//	const std::string& EditorWindow::GetName() const
//	{
//		return m_name;
//	}
//	int EditorWindow::GetID() const
//	{
//		return m_ID;
//	}
//	const std::string& EditorWindow::GetNameID() const
//	{
//		return m_nameID;
//	}
//	float EditorWindow::GetWidth() const
//	{
//		return m_width;
//	}
//	float EditorWindow::GetHeight() const
//	{
//		return m_height;
//	}
//	bool EditorWindow::WantCaptureEvents() const
//	{
//		return m_wantCaptureEvents;
//	}
//	void EditorWindow::Open()
//	{
//		m_isOpen = true;
//	}
//}