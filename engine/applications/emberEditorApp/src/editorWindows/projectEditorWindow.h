#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "gui.h"



namespace emberEditor
{
	struct ProjectEditorWindow : public emberCore::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberCore::Gui;



		ProjectEditorWindow()
		{
			m_name = "Project";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		void Render() override
		{
			Gui::TextUnformatted("test");
		}
	};
}