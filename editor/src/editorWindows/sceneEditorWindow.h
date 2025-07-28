#ifndef __INCLUDE_GUARD_sceneEditorWindow_h__
#define __INCLUDE_GUARD_sceneEditorWindow_h__
#include "dearImGui.h"
#include "editor.h"
#include "editorWindow.h"
#include "emberTime.h"
#include "renderPassManager.h"
#include "renderTexture2d.h"
#include "vulkanForwardRenderPass.h"
#include <imgui.h>



namespace emberEngine
{
	struct SceneEditorWindow : EditorWindow
	{
		SceneEditorWindow()
		{
			m_name = "Scene Editor Window";
			m_wantCaptureEvents = false;
		}

		void Render() override
		{
			ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				Texture2d* pRenderTexture = (Texture2d*)RenderPassManager::GetForwardRenderPass()->GetRenderTexture();
				ImTextureID textureID = DearImGui::GetTextureID(pRenderTexture);
				ImGui::Image(textureID, ImVec2((float)pRenderTexture->GetWidth(), (float)pRenderTexture->GetHeight()));
			}
			ImGui::End();
		}
	};
}



#endif // __INCLUDE_GUARD_sceneEditorWindow_h__