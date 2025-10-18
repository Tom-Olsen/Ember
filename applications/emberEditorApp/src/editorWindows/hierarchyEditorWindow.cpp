#include "hierarchyEditorWindow.h"
#include "commonGuiFlags.h"
#include "gameObject.h"
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
		m_windowFlags = emberCommon::GuiWindowFlag_None;
		m_wantCaptureEvents = true;
		m_nameID = m_name + "##" + std::to_string(m_ID);
		m_pScene = nullptr;
	}
	HierarchyEditorWindow::~HierarchyEditorWindow()
	{

	}

	void HierarchyEditorWindow::SetScene(emberEngine::Scene* pScene)
	{
		m_pScene = pScene;
	}

	void HierarchyEditorWindow::Render()
	{
		if (m_pScene != nullptr)
		{
			auto& gameObjects = m_pScene->GetGameObjects();
			for (auto& [name, pGameObject] : gameObjects)
				Gui::TextUnformatted(name.c_str());
		}
	}
}