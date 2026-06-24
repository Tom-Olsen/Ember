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
		emberEcs::Scene* m_pScene;

	public: // Methods
		HierarchyEditorWindow();
		~HierarchyEditorWindow();

		void SetScene(emberEcs::Scene* pScene);
		void Render() override;
	};
}