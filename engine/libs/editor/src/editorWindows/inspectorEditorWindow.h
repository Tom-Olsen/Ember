#pragma once
#include "commonGuiFlags.h"
#include "editorSelection.h"
#include "editorWindow.h"
#include "entity.h"
#include "gui.h"
#include "transform.h"



namespace emberEditor
{
	struct InspectorEditorWindow : public emberCore::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberCore::Gui;



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
				Gui::TextSelectable("Name", entity.GetName());
				Gui::TextSelectable("Position", entity.GetTransform()->GetPosition().ToString("%.4f"));
				Gui::TextSelectable("Rotation", entity.GetTransform()->GetRotation().ToString("%.4f"));
				Gui::TextSelectable("Scale", entity.GetTransform()->GetScale().ToString("%.4f"));
			}
			else
				Gui::TextUnformatted("No selection");
		}
	};
}