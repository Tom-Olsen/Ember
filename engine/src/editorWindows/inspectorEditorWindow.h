#ifndef __INCLUDE_GUARD_inspectorEditorWindow_h__
#define __INCLUDE_GUARD_inspectorEditorWindow_h__
#include "editor.h"
#include "editorWindow.h"



namespace emberEngine
{
	struct InspectorEditorWindow : EditorWindow
	{
	private: // Members

	public: // Methods
		InspectorEditorWindow()
		{
			m_name = "Inspector";
			m_ID = 0;
			m_windowFlags = WindowFlag::None;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		void Render() override
		{
			Editor::TextUnformatted("test");
		}
	};
}



#endif // __INCLUDE_GUARD_inspectorEditorWindow_h__