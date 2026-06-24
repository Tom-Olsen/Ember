#include "hierarchyEditorWindow.h"
#include "commonGuiFlags.h"
#include "editorSelection.h"
#include "entity.h"
#include "gui.h"
#include "scene.h"



namespace emberEditor
{
	// Easy access to emberEngine Gui:
	using Gui = emberEngine::Gui;



	// Constructor/Destructor:
	HierarchyEditorWindow::HierarchyEditorWindow()
	{
		m_name = "Hierarchy";
		m_ID = 0;
		m_windowFlags = emberCommon::GuiWindowFlags::none;
		m_wantCaptureEvents = true;
		m_nameID = m_name + "##" + std::to_string(m_ID);
		m_pScene = nullptr;
	}
	HierarchyEditorWindow::~HierarchyEditorWindow()
	{

	}

	void HierarchyEditorWindow::SetScene(emberEcs::Scene* pScene)
	{
		m_pScene = pScene;
	}

	void HierarchyEditorWindow::Render()
	{
		if (m_pScene == nullptr)
			return;

		std::vector<std::string> entityNames = m_pScene->GetEntityNames();
		for (const std::string& name : entityNames)
		{
			bool isSelected = emberEditor::EditorSelection::HasSelectedEntity()
                           && emberEditor::EditorSelection::GetSelectedEntity().GetName() == name;
			if (Gui::Selectable(name.c_str(), isSelected))
			{
				emberEcs::Entity entity = m_pScene->GetEntity(name);
				emberEditor::EditorSelection::SetSelectedEntity(entity);
			}
		}
	}
}