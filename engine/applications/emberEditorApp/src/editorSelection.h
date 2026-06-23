#pragma once
#include "entity.h"
#include <optional>



namespace emberEditor
{
	class EditorSelection
	{
	private: // Members:
		static std::optional<emberEngine::Entity> s_selectedEntity;

	public: // Methods:
		static void SetSelectedEntity(const emberEngine::Entity& entity);
		static emberEngine::Entity GetSelectedEntity();
		static bool HasSelectedEntity();
		static void ClearSelection();

	private: // Methods:
		EditorSelection() = delete;
		EditorSelection(const EditorSelection&) = delete;
		EditorSelection& operator=(const EditorSelection&) = delete;
		EditorSelection(EditorSelection&&) = delete;
		EditorSelection& operator=(EditorSelection&&) = delete;
		~EditorSelection() = delete;
	};
}
