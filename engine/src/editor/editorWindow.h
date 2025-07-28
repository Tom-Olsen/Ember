#ifndef __INCLUDE_GUARD_editorWindow_h__
#define __INCLUDE_GUARD_editorWindow_h__
#include <string>



namespace emberEngine
{
	/// <summary>
	/// Polymorphic parent struct for all editor windows.
	/// </summary>
	struct EditorWindow
	{
	protected: // Members:
		std::string m_name;
		bool m_wantCaptureEvents = true;

	public: // Methods:
		EditorWindow();
		virtual ~EditorWindow();
		
		virtual void Render();
		bool WantCaptureKeyboard() const;
		std::string GetName() const;
	};
}



#endif // __INCLUDE_GUARD_editorWindow_h__