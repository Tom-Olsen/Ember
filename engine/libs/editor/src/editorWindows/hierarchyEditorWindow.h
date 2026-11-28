#pragma once
#include "editorWindow.h"



namespace emberEditor
{
	struct HierarchyEditorWindow : public emberCore::EditorWindow
	{
	public: // Methods:
		// Constructor/Destructor:
		HierarchyEditorWindow();
		~HierarchyEditorWindow();

		// Overrides:
		void Render() override;
	};
}