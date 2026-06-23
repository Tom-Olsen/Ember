#include "editorSelection.h"
#include <assert.h>



namespace emberEditor
{
	// Static members:
	std::optional<emberEngine::Entity> EditorSelection::s_selectedEntity = std::nullopt;



	// Public methods:
	void EditorSelection::SetSelectedEntity(const emberEngine::Entity& entity)
	{
		s_selectedEntity = entity;
	}
	emberEngine::Entity EditorSelection::GetSelectedEntity()
	{
		assert(s_selectedEntity.has_value());
		return s_selectedEntity.value();
	}
	bool EditorSelection::HasSelectedEntity()
	{
		return s_selectedEntity.has_value() && s_selectedEntity->GetIsValid();
	}
	void EditorSelection::ClearSelection()
	{
		s_selectedEntity = std::nullopt;
	}
}
