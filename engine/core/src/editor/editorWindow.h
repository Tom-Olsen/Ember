#pragma once
#include "commonGuiFlags.h"
#include <memory>
#include <string>



namespace emberEngine
{
	class EditorWindow
	{
	protected: // Members:
		std::string m_name;							// set by derived classes in constructor.
		int m_ID;									// full id is "m_name##m_ID". For multiple windows with the same name, this ID must be unique.
		emberCommon::GuiWindowFlags m_windowFlags;	// set by derived classes in constructor.
		bool m_wantCaptureEvents = true;			// set by derived classes in constructor.
		std::string m_nameID;						// ideally set in inheritor constructor.

		// Automatically set by EditorWindow:
		bool m_isOpen = true;
		bool m_isFocused = false;
		float m_width = 0.0f;
		float m_height = 0.0f;

	public: // Methods:
		// Constructors/Destructor:
		EditorWindow();
		~EditorWindow();

		// Non-copyable:
		EditorWindow(const EditorWindow&) = delete;
		EditorWindow& operator=(const EditorWindow&) = delete;

		// Movable:
		EditorWindow(EditorWindow&& other) noexcept;
		EditorWindow& operator=(EditorWindow&& other) noexcept;

		// Rendering:
		void Open();
		void RenderWindow();		// gets calles by IEditor::Render() and calls overloaded IEditorWindow::Render().
		virtual void Render();		// override this method with your window content.

		// Getters:
		const std::string& GetName() const;
		int GetID() const;
		const std::string& GetNameID() const;
		float GetWidth() const;
		float GetHeight() const;
		bool WantCaptureEvents() const;
	};
}