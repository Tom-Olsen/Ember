#pragma once
#include "editorWindow.h"
#include "gui.h"
#include "renderer.h"
#include <string>



namespace emberEditor
{
	class DeferredLightingEditorWindow : public emberCore::EditorWindow
	{
	private: // Members:
		int m_renderMode = 0;

	public: // Methods:
		// Constructor:
		DeferredLightingEditorWindow()
		{
			m_name = "Deferred Lighting";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
			GetData();
		}

		// Overrides:
		void Render() override
		{
			GetData();

			// Indices match Settings.renderMode in pbrDeferredLighting.frag.hlsl:
			const char* renderModeNames[] = { "Lighting", "Albedo", "World normals", "Metallicity", "Roughness", "Ambient occlusion" };
			for (int renderMode = 0; renderMode < 6; renderMode++)
			{
				if (emberCore::Gui::Selectable(renderModeNames[renderMode], m_renderMode == renderMode))
					m_renderMode = renderMode;
			}

			SetData();
		}

	private: // Methods:
		// Data synchronization:
		void GetData()
		{
			m_renderMode = emberCore::Renderer::GetDeferredLightingRenderMode();
		}
		void SetData()
		{
			emberCore::Renderer::SetDeferredLightingRenderMode(m_renderMode);
		}
	};
}