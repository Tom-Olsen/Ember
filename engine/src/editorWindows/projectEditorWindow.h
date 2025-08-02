#ifndef __INCLUDE_GUARD_projectEditorWindow_h__
#define __INCLUDE_GUARD_projectEditorWindow_h__
#include "editor.h"
#include "editorWindow.h"
#include <imgui.h>



namespace emberEngine
{
	struct ProjectEditorWindow : EditorWindow
	{
	private: // Members

	public: // Methods
		ProjectEditorWindow()
		{
			m_name = "Project";
			m_ID = 0;
			m_windowFlags = ImGuiWindowFlags_None;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		void Render() override
		{
			Editor::TextUnformatted("test");
		}
	};
}



#endif // __INCLUDE_GUARD_projectEditorWindow_h__