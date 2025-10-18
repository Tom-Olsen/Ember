#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "gui.h"



namespace emberEditor
{
	struct InspectorEditorWindow : public emberEngine::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberEngine::Gui;



		InspectorEditorWindow()
		{
			m_name = "Inspector";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlag_None;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		void Render() override
		{
			Gui::TextUnformatted("test");
		}
	};
}