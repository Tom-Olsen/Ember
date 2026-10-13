#pragma once
#include "editorWindow.h"
#include "commonGuiFlags.h"
#include "emberTime.h"
#include "gui.h"



namespace emberEditor
{
	struct FpsEditorWindow : public emberCore::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberCore::Gui;

	public: // Methods
		FpsEditorWindow()
		{
			m_name = "Fps";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = false;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		void Render() override
		{
			float fps = emberCore::Time::GetFps();
			float frameTime = fps > 0.0f ? 1000.0f / fps : 0.0f;
			Gui::Text("FPS: %.1f", fps);
			Gui::Text("Frame Time: %.2fms", frameTime);
		}
	};
}