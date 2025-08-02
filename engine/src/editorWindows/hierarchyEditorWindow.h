#ifndef __INCLUDE_GUARD_hierarchyEditorWindow_h__
#define __INCLUDE_GUARD_hierarchyEditorWindow_h__
#include "editor.h"
#include "editorWindow.h"
#include <imgui.h>



namespace emberEngine
{
	// Forward decleration:
	class Scene;



	struct HierarchyEditorWindow : EditorWindow
	{
	private: // Members
		Scene* m_pScene;

	public: // Methods
		HierarchyEditorWindow();
		~HierarchyEditorWindow();

		void SetScene(Scene* pScene);
		void Render() override;
	};
}



#endif // __INCLUDE_GUARD_hierarchyEditorWindow_h__