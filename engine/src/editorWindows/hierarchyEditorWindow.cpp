#include "hierarchyEditorWindow.h"
#include "scene.h"
#include "gameObject.h"



namespace emberEngine
{
	// Constructor/Destructor:
	HierarchyEditorWindow::HierarchyEditorWindow()
	{
		m_name = "Hierarchy";
		m_ID = 0;
		m_windowFlags = ImGuiWindowFlags_None;
		m_wantCaptureEvents = true;
		m_nameID = m_name + "##" + std::to_string(m_ID);
		m_pScene = nullptr;
	}
	HierarchyEditorWindow::~HierarchyEditorWindow()
	{

	}

	void HierarchyEditorWindow::SetScene(Scene* pScene)
	{
		m_pScene = pScene;
	}

	void HierarchyEditorWindow::Render()
	{
		if (m_pScene != nullptr)
		{
			auto& gameObjects = m_pScene->GetGameObjects();

			for (auto& [name, pGameObject] : gameObjects)
			{
				Editor::TextUnformatted(name.c_str());
			}
		}
	}
}