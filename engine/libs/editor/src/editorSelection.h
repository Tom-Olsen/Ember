#pragma once
#include "entityHandle.h"



namespace emberEditor
{
	class EditorSelection
	{
	private: // Members:
		static emberEcs::EntityHandle s_selectedEntity;

	public: // Methods:
		static void SetSelectedEntity(emberEcs::EntityHandle entity);
		static emberEcs::EntityHandle GetSelectedEntity();
		static bool HasSelectedEntity();
		static void ClearSelection();
		static bool SelectEntityByMouse();

	private: // Methods:
		EditorSelection() = delete;
		EditorSelection(const EditorSelection&) = delete;
		EditorSelection& operator=(const EditorSelection&) = delete;
		EditorSelection(EditorSelection&&) = delete;
		EditorSelection& operator=(EditorSelection&&) = delete;
		~EditorSelection() = delete;
	};
}