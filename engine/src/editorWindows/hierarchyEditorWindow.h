#ifndef __INCLUDE_GUARD_hierarchyEditorWindow_h__
#define __INCLUDE_GUARD_hierarchyEditorWindow_h__
#include "editor.h"
#include "editorWindow.h"
#include <imgui.h>



namespace emberEngine
{
	struct HierarchyEditorWindow : EditorWindow
	{
	private: // Members

	public: // Methods
		HierarchyEditorWindow()
		{
			m_name = "Hierarchy";
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



#endif // __INCLUDE_GUARD_hierarchyEditorWindow_h__