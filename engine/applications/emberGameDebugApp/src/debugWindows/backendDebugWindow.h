#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "gui.h"
#include "renderer.h"



namespace emberEditor
{
	struct BackendDebugEditorWindow : public emberCore::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberCore::Gui;
		using Renderer = emberCore::Renderer;



	public: // Methods:
		BackendDebugEditorWindow()
		{
			m_name = "Backend Debug";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
		}

		void Render() override
		{
            if (Gui::Button("Dumb Buffer Allocation Info"))
                Renderer::DumpVmaBufferAllocations();
            if (Gui::Button("Dumb Image Allocation Info"))
                Renderer::DumpVmaImageAllocations();
		}
	};
}