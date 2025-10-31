#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "gui.h"
#include "renderer.h"



namespace emberEditor
{
	struct DepthBiasEditorWindow : public emberEngine::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberEngine::Gui;
		using Renderer = emberEngine::Renderer;



	private: // Members:
		float m_depthBiasConstantFactor;
		float m_depthBiasClamp;
		float m_depthBiasSlopeFactor;

	public: // Methods:
		DepthBiasEditorWindow()
		{
			m_name = "Depth Bias";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlag_None;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
			GetData();
		}

		void Render() override
		{
			GetData();
			Gui::DragFloat("Const Factor:", &m_depthBiasConstantFactor);
			Gui::DragFloat("Clamp Factor:", &m_depthBiasClamp);
			Gui::DragFloat("Slope Factor:", &m_depthBiasSlopeFactor);
			SetData();
		}

	private: // Methods:
		void GetData()
		{
			m_depthBiasConstantFactor = Renderer::GetDepthBiasConstantFactor();
			m_depthBiasClamp = Renderer::GetDepthBiasClamp();
			m_depthBiasSlopeFactor = Renderer::GetDepthBiasSlopeFactor();
		}
		void SetData()
		{
			Renderer::SetDepthBiasConstantFactor(m_depthBiasConstantFactor);
			Renderer::SetDepthBiasClamp(m_depthBiasClamp);
			Renderer::SetDepthBiasSlopeFactor(m_depthBiasSlopeFactor);
		}
	};
}