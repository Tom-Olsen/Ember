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

	private:
		Float3 m_position;
		Float3 m_rotation;
		Float3 m_scale;

	public:
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
				GetData(entity);
				Gui::TextSelectable("Name", entity.GetName());
				Gui::DragFloat3("Position:", &m_position, 0, 0, "%.4f");
				Gui::DragFloat3("Rotation:", &m_rotation, 0, 0, "%.4f");
				Gui::DragFloat3("Scale:", &m_scale, 0, 0, "%.4f");
				SetData(entity);
			}
			else
				Gui::TextUnformatted("No selection");
		}

	private:
		void GetData(emberEcs::Entity entity)
		{
			if (entity.GetIsValid())
			{
				m_position = entity.GetTransform()->GetPosition();
				m_rotation = entity.GetTransform()->GetRotation().GetValue();
				m_scale= entity.GetTransform()->GetScale();
			}
		}
		void SetData(emberEcs::Entity entity)
		{
			if (entity.GetIsValid())
			{
				entity.GetTransform()->SetPosition(m_position);
				entity.GetTransform()->SetRotationEuler(EulerDegrees(m_rotation));
				entity.GetTransform()->SetScale(m_scale);
			}
		}
	};
}