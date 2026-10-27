#pragma once
#include "editorWindow.h"
#include "entity.inl"
#include "gui.h"
#include "screenSpaceAmbientOcclusionDispatcher.h"
#include "ssaoConstants.h"
#include <algorithm>
#include <string>



namespace emberEditor
{
	class ScreenSpaceAmbientOcclusionDispatcherEditorWindow : public emberCore::EditorWindow
	{
	private: // Members:
		emberEcs::Scene* m_pScene = nullptr;

	public: // Methods:
		// Constructor:
		ScreenSpaceAmbientOcclusionDispatcherEditorWindow()
		{
			m_name = "Screen Space Ambient Occlusion";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		// Setters:
		void SetScene(emberEcs::Scene* pScene)
		{
			m_pScene = pScene;
		}

		// Overrides:
		void Render() override
		{
			if (m_pScene == nullptr)
				return;

			// Find dispatcher:
			emberEcs::ScreenSpaceAmbientOcclusionDispatcher* pDispatcher = nullptr;
			for (const std::string& entityName : m_pScene->GetEntityNames())
			{
				emberEcs::Entity entity = m_pScene->GetEntity(entityName);
				if (entity.HasComponent<emberEcs::ScreenSpaceAmbientOcclusionDispatcher>())
				{
					pDispatcher = entity.GetComponent<emberEcs::ScreenSpaceAmbientOcclusionDispatcher>();
					break;
				}
			}

			if (pDispatcher == nullptr)
			{
				emberCore::Gui::TextUnformatted("No screen-space ambient occlusion dispatcher found.");
				return;
			}

			// SSAO mode selection:
			using SsaoMode = emberEcs::ScreenSpaceAmbientOcclusionDispatcher::SsaoMode;
			if (emberCore::Gui::Selectable("Off", pDispatcher->ssaoMode == SsaoMode::off))
				pDispatcher->ssaoMode = SsaoMode::off;
			if (emberCore::Gui::Selectable("Full resolution", pDispatcher->ssaoMode == SsaoMode::fullResolution))
				pDispatcher->ssaoMode = SsaoMode::fullResolution;
			if (emberCore::Gui::Selectable("Half resolution", pDispatcher->ssaoMode == SsaoMode::halfResolution))
				pDispatcher->ssaoMode = SsaoMode::halfResolution;

			// Shared SSAO sampling parameters:
			emberCore::Gui::SeparatorText("Sampling parameters");
			if (emberCore::Gui::DragInt("Sample count", &pDispatcher->sampleCount, 1))
				pDispatcher->sampleCount = std::clamp(pDispatcher->sampleCount, 1, SSAO_MAX_SAMPLE_COUNT);
			if (emberCore::Gui::DragFloat("Sample radius (world units)", &pDispatcher->sampleRadius, 0.01f, 0.1f))
				pDispatcher->sampleRadius = std::max(0.0001f, pDispatcher->sampleRadius);
			if (emberCore::Gui::DragFloat("Sample bias (world units)", &pDispatcher->sampleBias, 0.001f, 0.01f))
				pDispatcher->sampleBias = std::max(0.0f, pDispatcher->sampleBias);
			if (emberCore::Gui::DragFloat("Occlusion strength", &pDispatcher->occlusionStrength, 0.1f, 1.0f))
				pDispatcher->occlusionStrength = std::max(0.0f, pDispatcher->occlusionStrength);
		}
	};
}