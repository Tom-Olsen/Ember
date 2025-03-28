#ifndef __INCLUDE_GUARD_editorWindow_h__
#define __INCLUDE_GUARD_editorWindow_h__



namespace emberEngine
{
	/// <summary>
	/// Polymorphic parent struct for all editor windows.
	/// </summary>
	struct EditorWindow
	{
		EditorWindow();
		virtual ~EditorWindow();
		virtual void Render();
	};
}



#endif // __INCLUDE_GUARD_editorWindow_h__