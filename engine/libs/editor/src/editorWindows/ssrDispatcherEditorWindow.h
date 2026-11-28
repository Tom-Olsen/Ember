#pragma once
#include "editorWindow.h"
#include "commonGuiFlags.h"
#include "entity.inl"
#include "gui.h"
#include "ssrDispatcher.h"
#include "world.h"
#include <string>



namespace emberEditor
{
	struct SsrDispatcherEditorWindow : public emberCore::EditorWindow
	{
	private: // Members:
		emberEcs::SsrDispatcher* m_pDispatcher = nullptr;
		emberEcs::SsrDispatcher::SsrMode m_ssrMode;

	public: // Methods:
		// Constructor:
		SsrDispatcherEditorWindow()
		{
			m_name = "Screen Space Reflections";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		// Overrides:
		void Render() override
		{
			// Find dispatcher:
			m_pDispatcher = nullptr;
			for (emberEcs::EntityHandle handle : emberEcs::World::GetEntities())
			{
				emberEcs::Entity& entity = *handle.Get();
				if (entity.HasComponent<emberEcs::SsrDispatcher>())
				{
					m_pDispatcher = entity.GetComponent<emberEcs::SsrDispatcher>();
					break;
				}
			}

			// Fallback:
			if (m_pDispatcher == nullptr)
			{
				emberCore::Gui::TextUnformatted("No screen-space reflection dispatcher found.");
				return;
			}

			GetData();

			// Ssr mode selection:
			if (emberCore::Gui::Selectable("Off", m_ssrMode == emberEcs::SsrDispatcher::SsrMode::off))
				m_ssrMode = emberEcs::SsrDispatcher::SsrMode::off;
			if (emberCore::Gui::Selectable("Full resolution", m_ssrMode == emberEcs::SsrDispatcher::SsrMode::fullResolution))
				m_ssrMode = emberEcs::SsrDispatcher::SsrMode::fullResolution;
			if (emberCore::Gui::Selectable("Reflection map", m_ssrMode == emberEcs::SsrDispatcher::SsrMode::reflectionMap))
				m_ssrMode = emberEcs::SsrDispatcher::SsrMode::reflectionMap;
			if (emberCore::Gui::Selectable("Reflection UV map", m_ssrMode == emberEcs::SsrDispatcher::SsrMode::reflectionUVMap))
				m_ssrMode = emberEcs::SsrDispatcher::SsrMode::reflectionUVMap;

			SetData();
		}

	private: // Methods:
		// Data synchronization:
		void GetData()
		{
			m_ssrMode = m_pDispatcher->ssrMode;
		}
		void SetData()
		{
			m_pDispatcher->ssrMode = m_ssrMode;
		}
	};
}