#ifndef __INCLUDE_GUARD_editor_h__
#define __INCLUDE_GUARD_editor_h__
#include <memory>
#include <unordered_set>



namespace emberEngine
{
	// Forward declerations:
	class EditorWindow;



	/// <summary>
	/// Static class that manages all editor windows and hands them over to dearImGui for rendering.
	/// </summary>
	class Editor
	{
	private: // Members:
		static std::unordered_set<EditorWindow*> s_editorWindows;

	public: // Methods:
		static void Init();
		static void Clear();

		static void Render();
		static void AddEditorWindow(EditorWindow* pEditorWindow);
		static void DeleteEditorWindow(EditorWindow* pEditorWindow);
	};
}



#endif // __INCLUDE_GUARD_editor_h__