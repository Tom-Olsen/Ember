#pragma once
#include "commonGuiFlags.h"
#include "editorSelection.h"
#include "editorWindow.h"
#include "entity.h"
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
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		void Render() override
		{
			if (emberEditor::EditorSelection::HasSelectedEntity())
			{
				emberEcs::Entity entity = emberEditor::EditorSelection::GetSelectedEntity();
				Gui::TextUnformatted(entity.GetName().c_str());
			}
			else
				Gui::TextUnformatted("No selection");
		}
	};
}