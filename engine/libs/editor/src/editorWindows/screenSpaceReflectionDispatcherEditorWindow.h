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
			if (emberCore::Gui::Selectable("Off", pDispatcher->ssrMode == emberEcs::ScreenSpaceReflectionDispatcher::SsrMode::off))
				pDispatcher->ssrMode = emberEcs::ScreenSpaceReflectionDispatcher::SsrMode::off;
			if (emberCore::Gui::Selectable("Full resolution", pDispatcher->ssrMode == emberEcs::ScreenSpaceReflectionDispatcher::SsrMode::fullResolution))
				pDispatcher->ssrMode = emberEcs::ScreenSpaceReflectionDispatcher::SsrMode::fullResolution;
			if (emberCore::Gui::Selectable("Reflection map", pDispatcher->ssrMode == emberEcs::ScreenSpaceReflectionDispatcher::SsrMode::reflectionMap))
				pDispatcher->ssrMode = emberEcs::ScreenSpaceReflectionDispatcher::SsrMode::reflectionMap;
			if (emberCore::Gui::Selectable("Reflection UV map", pDispatcher->ssrMode == emberEcs::ScreenSpaceReflectionDispatcher::SsrMode::reflectionUVMap))
				pDispatcher->ssrMode = emberEcs::ScreenSpaceReflectionDispatcher::SsrMode::reflectionUVMap;
		}
	};
}