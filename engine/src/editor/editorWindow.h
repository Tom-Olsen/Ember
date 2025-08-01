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
		int m_ID;							// full id is "m_name##m_ID". For multiple windows with the same name, this ID must be unique.
		ImGuiWindowFlags m_windowFlags;		// set by derived classes in constructor.
		bool m_wantCaptureEvents = true;	// set by derived classes in constructor.
		std::string m_nameID;				// ideally set in inheritor constructor.

		// Automatically set by EditorWindow:
		bool m_isOpen = true;
		bool m_isFocused = false;
		float m_width = 0.0f;
		float m_height = 0.0f;

	public: // Methods:
		EditorWindow();
		virtual ~EditorWindow();
		
		void BaseRender();		// This gets called by Editor::Render() and calls Render()
		virtual void Render();	// This gets overridden by derived classes
		
		const std::string& GetName() const;
		int GetID() const;
		const std::string& GetNameID() const;
		float GetWidth() const;
		float GetHeight() const;
		bool WantCaptureEvents() const;
		void Open();
	};
}



#endif // __INCLUDE_GUARD_editorWindow_h__