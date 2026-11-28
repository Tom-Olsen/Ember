#include "hierarchyEditorWindow.h"
#include "commonGuiFlags.h"
#include "editorSelection.h"
#include "entity.h"
#include "gui.h"
#include "world.h"
#include <algorithm>



namespace emberEditor
{
	// Easy access to emberEngine Gui:
	using Gui = emberCore::Gui;



	// Public Methods:
	// Constructor/Destructor:
	HierarchyEditorWindow::HierarchyEditorWindow()
	{
		m_name = "Hierarchy";
		m_ID = 0;
		m_windowFlags = emberCommon::GuiWindowFlags::none;
		m_wantCaptureEvents = true;
		m_nameID = m_name + "##" + std::to_string(m_ID);
	}
	HierarchyEditorWindow::~HierarchyEditorWindow()
	{

	}



	// Overrides:
	void HierarchyEditorWindow::Render()
	{
		std::vector<emberEcs::EntityHandle> entities = emberEcs::World::GetEntities();
		std::sort(entities.begin(), entities.end(), [](emberEcs::EntityHandle left, emberEcs::EntityHandle right)
		{
			const emberEcs::Entity& leftEntity = *left.Get();
			const emberEcs::Entity& rightEntity = *right.Get();
			if (leftEntity.GetName() == rightEntity.GetName())
				return leftEntity.GetId() < rightEntity.GetId();
			return leftEntity.GetName() < rightEntity.GetName();
		});

		for (emberEcs::EntityHandle handle : entities)
		{
			const emberEcs::Entity& entity = *handle.Get();
			const std::string guiId = std::to_string(entity.GetId());
			Gui::PushID(guiId.c_str());
			if (Gui::Selectable(entity.GetName().c_str(), EditorSelection::GetSelectedEntity() == handle))
				EditorSelection::SetSelectedEntity(handle);
			Gui::PopID();
		}
	}
}