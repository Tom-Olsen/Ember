#include "fpsEditorWindow.h"
#include "editor.h"
#include "emberTime.h"
#include "logger.h"
#include <imgui.h>



namespace emberEngine
{
	void FpsEditorWindow::Render() const
	{
		float fps = 1.0f / Time::GetDeltaTime();
		ImGui::Begin("Performance");
		ImGui::Text("FPS: %.1f", fps);
		ImGui::End();
	}
}