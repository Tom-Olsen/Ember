#pragma once
#include "entity.h"
#include <optional>



namespace emberEcs
{
	class Scene;
}



namespace emberEditor
{
	class EditorSelection
	{
	private: // Members:
		static std::optional<emberEcs::Entity> s_selectedEntity;

	public: // Methods:
		static void SetSelectedEntity(const emberEcs::Entity& entity);
		static emberEcs::Entity GetSelectedEntity();
		static bool HasSelectedEntity();
		static void ClearSelection();
		static bool SelectEntityByMouse(emberEcs::Scene& scene);

	private: // Methods:
		EditorSelection() = delete;
		EditorSelection(const EditorSelection&) = delete;
		EditorSelection& operator=(const EditorSelection&) = delete;
		EditorSelection(EditorSelection&&) = delete;
		EditorSelection& operator=(EditorSelection&&) = delete;
		~EditorSelection() = delete;
	};
}