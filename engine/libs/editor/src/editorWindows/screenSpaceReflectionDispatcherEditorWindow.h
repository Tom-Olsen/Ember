#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "entity.inl"
#include "gui.h"
#include "screenSpaceReflectionDispatcher.h"



namespace emberEditor
{
	struct ScreenSpaceReflectionDispatcherEditorWindow : public emberCore::EditorWindow
	{
	private: // Members:
		emberEcs::Scene* m_pScene;

	public: // Methods:
		ScreenSpaceReflectionDispatcherEditorWindow()
		{
			m_name = "Screen Space Reflections";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
			m_pScene = nullptr;
		}

		void SetScene(emberEcs::Scene* pScene)
		{
			m_pScene = pScene;
		}

		void Render() override
		{
			if (m_pScene == nullptr)
				return;

			// Find dispatcher:
			emberEcs::ScreenSpaceReflectionDispatcher* pDispatcher = nullptr;
			for (const std::string& entityName : m_pScene->GetEntityNames())
			{
				emberEcs::Entity entity = m_pScene->GetEntity(entityName);
				if (entity.HasComponent<emberEcs::ScreenSpaceReflectionDispatcher>())
				{
					pDispatcher = entity.GetComponent<emberEcs::ScreenSpaceReflectionDispatcher>();
					break;
				}
			}

			// Fallback:
			if (pDispatcher == nullptr)
			{
				emberCore::Gui::TextUnformatted("No screen-space reflection dispatcher found.");
				return;
			}

			// Ssr mode selection:
			if (emberCore::Gui::Selectable("0 - Full resolution", pDispatcher->ssrMode == 0))
				pDispatcher->ssrMode = 0;
			if (emberCore::Gui::Selectable("1 - Reflection map", pDispatcher->ssrMode == 1))
				pDispatcher->ssrMode = 1;
			if (emberCore::Gui::Selectable("2 - Reflection UV map", pDispatcher->ssrMode == 2))
				pDispatcher->ssrMode = 2;
		}
	};
}