#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "gui.h"
#include "renderer.h"



namespace emberEditor
{
	struct OutlineEditorWindow : public emberCore::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberCore::Gui;
		using Renderer = emberCore::Renderer;

	private: // Members:
        int m_outlineThickness;
		Float4 m_outlineColor;

	public: // Methods:
		OutlineEditorWindow()
		{
			m_name = "Outline";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
			GetData();
		}
		void Render() override
		{
			GetData();
			Gui::DragInt("Thickness:", &m_outlineThickness);
			Gui::ColorEdit("Color:", &m_outlineColor);
			SetData();
		}

	private: // Methods:
		void GetData()
		{
			m_outlineThickness = Renderer::GetOutlineThickness();
			m_outlineColor = Renderer::GetOutlineColor();
		}
		void SetData()
		{
			Renderer::SetOutlineThickness(m_outlineThickness);
			Renderer::SetOutlineColor(m_outlineColor);
		}
	};
}