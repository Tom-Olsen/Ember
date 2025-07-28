#ifndef __INCLUDE_GUARD_fpsEditorWindow_h__
#define __INCLUDE_GUARD_fpsEditorWindow_h__
#include "editor.h"
#include "editorWindow.h"
#include "emberTime.h"
#include <imgui.h>



namespace emberEngine
{
	struct FpsEditorWindow : EditorWindow
	{
		float m_frameTimes[60] = {};
		int m_index = 0;
		float m_frameTime;
		float m_fps;
		float m_deltaTimeSum = 0.0f;
		int m_rampUpIndex = 1;

		FpsEditorWindow()
		{
			m_name = "Fps Editor Window";
		}

		void Render() override
		{
			m_deltaTimeSum -= m_frameTimes[m_index];
			m_frameTimes[m_index] = Time::GetDeltaTime();
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

			ImGui::Begin("Performance");
			{
				Editor::Text("FPS: %.1f", m_fps);
				Editor::Text("Frame Time: %.0fms", 1000 * m_frameTime);
			}
			ImGui::End();
		}
	};
}



#endif // __INCLUDE_GUARD_fpsEditorWindow_h__