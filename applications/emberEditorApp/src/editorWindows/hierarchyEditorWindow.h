#pragma once
#include "editorWindow.h"
#include "gui.h"



// Forward decleration:
namespace emberEngine
{
	class Scene;
}



namespace emberEditor
{
	struct HierarchyEditorWindow : public emberEngine::EditorWindow
	{
	private: // Members
		emberEngine::Scene* m_pScene;

	public: // Methods
		HierarchyEditorWindow();
		~HierarchyEditorWindow();

		void SetScene(emberEngine::Scene* pScene);
		void Render() override;
	};
}