#ifndef __INCLUDE_GUARD_depthBiasEditorWindow_h__
#define __INCLUDE_GUARD_depthBiasEditorWindow_h__
#include "editor.h"
#include "editorWindow.h"
#include "graphics.h"
#include <imgui.h>



namespace emberEngine
{
	struct DepthBiasEditorWindow : EditorWindow
	{
	private: // Members:
		float m_depthBiasConstantFactor;
		float m_depthBiasClamp;
		float m_depthBiasSlopeFactor;


	public: // Methods:
		DepthBiasEditorWindow()
		{
			GetData();
		}
		void Render() override
		{
			GetData();
			ImGui::Begin("Depth Bias");
			{
				Editor::InputFloat("Depth Bias Const Factor:", &m_depthBiasConstantFactor);
				Editor::InputFloat("Depth Bias Clamp Factor:", &m_depthBiasClamp);
				Editor::InputFloat("Depth Bias Slope Factor:", &m_depthBiasSlopeFactor);
			}
			ImGui::End();
			SetData();
		}

	private: // Methods:
		void GetData()
		{
			m_depthBiasConstantFactor = Graphics::GetDeptBiasConstantFactor();
			m_depthBiasClamp = Graphics::GetDeptBiasClamp();
			m_depthBiasSlopeFactor = Graphics::GetDeptBiasSlopeFactor();
		}
		void SetData()
		{
			Graphics::SetDeptBiasConstantFactor(m_depthBiasConstantFactor);
			Graphics::SetDeptBiasClamp(m_depthBiasClamp);
			Graphics::SetDeptBiasSlopeFactor(m_depthBiasSlopeFactor);
		}
	};
}



#endif // __INCLUDE_GUARD_depthBiasEditorWindow_h__