#ifndef __INCLUDE_GUARD_editorWindow_h__
#define __INCLUDE_GUARD_editorWindow_h__



namespace emberEngine
{
	/// <summary>
	/// Polymorphic parent class for all editor windows.
	/// </summary>
	class EditorWindow
	{
	private: // Members:

	public: // Methods:
		EditorWindow();
		virtual ~EditorWindow();

		virtual void Render() const;
	};
}



#endif // __INCLUDE_GUARD_editorWindow_h__