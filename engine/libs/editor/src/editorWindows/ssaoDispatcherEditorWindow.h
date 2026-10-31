#pragma once
#include "editorWindow.h"
#include "entity.inl"
#include "gui.h"
#include "ssaoDispatcher.h"
#include <string>



namespace emberEditor
{
	class SsaoDispatcherEditorWindow : public emberCore::EditorWindow
	{
	private: // Members:
		emberEcs::Scene* m_pScene = nullptr;
		emberEcs::SsaoDispatcher* m_pDispatcher = nullptr;
		// Settings:
		bool m_ssaoEnabled;
		float m_resolutionScale;
		// Sampling parameters:
		int m_sampleCount;
		float m_sampleRadius;
		float m_sampleBias;
		float m_occlusionStrength;
		// Temporal reconstruction parameters:
		float m_temporalHistoryWeight;
		float m_temporalMinimumDepthTolerance;
		float m_temporalRelativeDepthTolerance;
		float m_temporalMinimumNormalSimilarity;
		int m_temporalRngSeedModulo;
		// Bilateral blur parameters:
		int m_blurRadius;
		float m_blurSpatialWeightScale;
		float m_blurMinimumDepthTolerance;
		float m_blurRelativeDepthTolerance;
		float m_blurNormalWeightExponent;

	public: // Methods:
		// Constructor:
		SsaoDispatcherEditorWindow()
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
			m_pDispatcher = nullptr;
		}

		// Overrides:
		void Render() override
		{
			if (m_pScene == nullptr)
				return;

			// Find dispatcher:
			m_pDispatcher = nullptr;
			for (const std::string& entityName : m_pScene->GetEntityNames())
			{
				emberEcs::Entity entity = m_pScene->GetEntity(entityName);
				if (entity.HasComponent<emberEcs::SsaoDispatcher>())
				{
					m_pDispatcher = entity.GetComponent<emberEcs::SsaoDispatcher>();
					break;
				}
			}

			if (m_pDispatcher == nullptr)
			{
				emberCore::Gui::TextUnformatted("No screen-space ambient occlusion dispatcher found.");
				return;
			}

			GetData();

			// Settings:
			emberCore::Gui::Checkbox("SSAO enabled", &m_ssaoEnabled);
			emberCore::Gui::DragFloat("Resolution scale", &m_resolutionScale, 0.01f, 0.1f, "%.4f");

			// Sampling parameters:
			emberCore::Gui::SeparatorText("Sampling parameters");
			emberCore::Gui::DragInt("Sample count", &m_sampleCount, 1);
			emberCore::Gui::DragFloat("Sample radius", &m_sampleRadius, 0.01f, 0.1f);
			emberCore::Gui::DragFloat("Sample bias", &m_sampleBias, 0.001f, 0.01f);
			emberCore::Gui::DragFloat("Occlusion strength", &m_occlusionStrength, 0.1f, 1.0f);

			// Temporal reconstruction parameters:
			emberCore::Gui::SeparatorText("Temporal reconstruction");
			emberCore::Gui::DragFloat("History weight", &m_temporalHistoryWeight, 0.01f, 0.1f);
			emberCore::Gui::DragFloat("Minimum depth tolerance", &m_temporalMinimumDepthTolerance, 0.001f, 0.01f);
			emberCore::Gui::DragFloat("Relative depth tolerance", &m_temporalRelativeDepthTolerance, 0.001f, 0.01f);
			emberCore::Gui::DragFloat("Minimum normal similarity", &m_temporalMinimumNormalSimilarity, 0.01f, 0.1f);
			emberCore::Gui::DragInt("Seed modulo", &m_temporalRngSeedModulo, 1);

			// Bilateral blur parameters:
			emberCore::Gui::SeparatorText("Blur and reconstruction");
			emberCore::Gui::TextUnformatted("Blur radius is measured in AO texels.");
			emberCore::Gui::DragInt("Blur radius", &m_blurRadius, 1);
			emberCore::Gui::DragFloat("Spatial weight scale", &m_blurSpatialWeightScale, 0.01f, 0.1f);
			emberCore::Gui::DragFloat("Blur minimum depth tolerance (world units)", &m_blurMinimumDepthTolerance, 0.001f, 0.01f);
			emberCore::Gui::DragFloat("Blur relative depth tolerance", &m_blurRelativeDepthTolerance, 0.001f, 0.01f);
			emberCore::Gui::DragFloat("Normal weight exponent", &m_blurNormalWeightExponent, 1.0f, 8.0f);

			SetData();
		}

	private: // Methods:
		// Data synchronization:
		void GetData()
		{
			// Settings:
			m_ssaoEnabled = m_pDispatcher->GetSsaoEnabled();
			m_resolutionScale = m_pDispatcher->GetResolutionScale();
			// Sampling parameters:
			m_sampleCount = m_pDispatcher->GetSampleCount();
			m_sampleRadius = m_pDispatcher->GetSampleRadius();
			m_sampleBias = m_pDispatcher->GetSampleBias();
			m_occlusionStrength = m_pDispatcher->GetOcclusionStrength();
			// Temporal reconstruction parameters:
			m_temporalHistoryWeight = m_pDispatcher->GetTemporalHistoryWeight();
			m_temporalMinimumDepthTolerance = m_pDispatcher->GetTemporalMinimumDepthTolerance();
			m_temporalRelativeDepthTolerance = m_pDispatcher->GetTemporalRelativeDepthTolerance();
			m_temporalMinimumNormalSimilarity = m_pDispatcher->GetTemporalMinimumNormalSimilarity();
			m_temporalRngSeedModulo = m_pDispatcher->GetTemporalRngSeedModulo();
			// Bilateral blur parameters:
			m_blurRadius = m_pDispatcher->GetBlurRadius();
			m_blurSpatialWeightScale = m_pDispatcher->GetBlurSpatialWeightScale();
			m_blurMinimumDepthTolerance = m_pDispatcher->GetBlurMinimumDepthTolerance();
			m_blurRelativeDepthTolerance = m_pDispatcher->GetBlurRelativeDepthTolerance();
			m_blurNormalWeightExponent = m_pDispatcher->GetBlurNormalWeightExponent();
		}
		void SetData()
		{
			// Settings:
			m_pDispatcher->SetSsaoEnabled(m_ssaoEnabled);
			m_pDispatcher->SetResolutionScale(m_resolutionScale);
			// Sampling parameters:
			m_pDispatcher->SetSampleCount(m_sampleCount);
			m_pDispatcher->SetSampleRadius(m_sampleRadius);
			m_pDispatcher->SetSampleBias(m_sampleBias);
			m_pDispatcher->SetOcclusionStrength(m_occlusionStrength);
			// Temporal reconstruction parameters:
			m_pDispatcher->SetTemporalHistoryWeight(m_temporalHistoryWeight);
			m_pDispatcher->SetTemporalMinimumDepthTolerance(m_temporalMinimumDepthTolerance);
			m_pDispatcher->SetTemporalRelativeDepthTolerance(m_temporalRelativeDepthTolerance);
			m_pDispatcher->SetTemporalMinimumNormalSimilarity(m_temporalMinimumNormalSimilarity);
			m_pDispatcher->SetTemporalRngSeedModulo(m_temporalRngSeedModulo);
			// Bilateral blur parameters:
			m_pDispatcher->SetBlurRadius(m_blurRadius);
			m_pDispatcher->SetBlurSpatialWeightScale(m_blurSpatialWeightScale);
			m_pDispatcher->SetBlurMinimumDepthTolerance(m_blurMinimumDepthTolerance);
			m_pDispatcher->SetBlurRelativeDepthTolerance(m_blurRelativeDepthTolerance);
			m_pDispatcher->SetBlurNormalWeightExponent(m_blurNormalWeightExponent);
		}
	};
}