#ifndef __INCLUDE_GUARD_editorWindow_h__
#define __INCLUDE_GUARD_editorWindow_h__
#include <imgui.h>
#include <string>



namespace emberEngine
{
	/// <summary>
	/// Polymorphic parent struct for all editor windows.
	/// </summary>
	struct EditorWindow
	{
	protected: // Members:
		std::string m_name;					// set by derived classes in constructor.
		ImGuiWindowFlags m_windowFlags;		// set by derived classes in constructor.
		bool m_wantCaptureEvents = true;	// set by derived classes in constructor.
		bool m_isOpen = true;				// tracks if window is open or closed by user.
		bool m_isFocused = false;			// tracks if window is in focus or not.

	public: // Methods:
		EditorWindow();
		virtual ~EditorWindow();
		
		void BaseRender();		// This gets called by Editor::Render() and calls Render()
		virtual void Render();	// This gets overridden by derived classes
		
		std::string GetName() const;
		bool WantCaptureEvents() const;
		void Open();
	};
}



#endif // __INCLUDE_GUARD_editorWindow_h__