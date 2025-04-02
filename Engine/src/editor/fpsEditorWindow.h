#ifndef __INCLUDE_GUARD_fpsEditorWindow_h__
#define __INCLUDE_GUARD_fpsEditorWindow_h__
#include "editorWindow.h"
#include "emberTime.h"
#include <imgui.h>



namespace emberEngine
{
	struct FpsEditorWindow : EditorWindow
	{
		float frameTimes[60] = {};
		int index = 0;
		float frameTime;
		float fps;
		float deltaTimeSum = 0.0f;
		int rampUpIndex = 1;

		void Render() override
		{
			deltaTimeSum -= frameTimes[index];
			frameTimes[index] = Time::GetDeltaTime();
			deltaTimeSum += frameTimes[index];
			index = (index + 1) % 60;
			if (rampUpIndex < 60)
			{
				fps = rampUpIndex / deltaTimeSum;
				frameTime = 1.0f / fps;
				rampUpIndex++;
			}
			else
			{
				fps = 60.0f / deltaTimeSum;
				frameTime = 1.0f / fps;
			}

			ImGui::Begin("Performance");
			ImGui::Text("FPS: %.1f", fps);
			ImGui::Text("Frame Time: %.0fms", 1000 * frameTime);
			ImGui::End();
		}
	};
}



#endif // __INCLUDE_GUARD_fpsEditorWindow_h__