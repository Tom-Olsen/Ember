#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "emberTime.h"
#include "gui.h"



namespace emberEditor
{
	struct FpsEditorWindow : public emberEngine::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberEngine::Gui;



	private: // Members
		float m_frameTimes[60] = {};
		int m_index = 0;
		float m_frameTime = 0.0f;
		float m_fps = 0.0f;
		float m_deltaTimeSum = 0.0f;
		int m_rampUpIndex = 1;

	public: // Methods
		FpsEditorWindow()
		{
			m_name = "Fps";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlag_None;
			m_wantCaptureEvents = false;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		void Render() override
		{
			m_deltaTimeSum -= m_frameTimes[m_index];
			m_frameTimes[m_index] = emberEngine::Time::GetDeltaTime();
			m_deltaTimeSum += m_frameTimes[m_index];
			m_index = (m_index + 1) % 60;
			if (m_rampUpIndex < 60)
			{
				m_fps = m_rampUpIndex / m_deltaTimeSum;
				m_frameTime = 1.0f / m_fps;
				m_rampUpIndex++;
			}
			else
			{
				m_fps = 60.0f / m_deltaTimeSum;
				m_frameTime = 1.0f / m_fps;
			}

			Gui::Text("FPS: %.1f", m_fps);
			Gui::Text("Frame Time: %.0fms", 1000 * m_frameTime);
		}
	};
}