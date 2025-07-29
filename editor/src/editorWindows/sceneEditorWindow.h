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
			m_name = "Scene";
			m_windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			m_wantCaptureEvents = false;
		}

        void Render() override
        {
            // Prepare render texture:
            Texture2d* pRenderTexture = (Texture2d*)RenderPassManager::GetForwardRenderPass()->GetRenderTexture();
            ImTextureID textureID = DearImGui::GetTextureID(pRenderTexture);

            // Get imgui window:
            const ImVec2 windowSize = ImGui::GetContentRegionAvail();

            // Texture and window aspect ratios:
            const float textureAspect = (float)pRenderTexture->GetWidth() / (float)pRenderTexture->GetHeight();
            const float windowAspect = windowSize.x / windowSize.y;

            ImVec2 imageSize;
            if (windowAspect > textureAspect)
            {
                // Window is wider than texture aspect ratio -> letterboxing:
                imageSize.y = windowSize.y;
                imageSize.x = imageSize.y * textureAspect;
            }
            else
            {
                // Window is taller than texture aspect ratio -> pillarboxing:
                imageSize.x = windowSize.x;
                imageSize.y = imageSize.x / textureAspect;
            }

            // Center the image in the window:
            ImVec2 cursorPos = ImGui::GetCursorPos();
            ImVec2 offset = { 0.5f * (windowSize.x - imageSize.x), 0.5f * (windowSize.y - imageSize.y) };
            ImGui::SetCursorPos(ImVec2(cursorPos.x + offset.x, cursorPos.y + offset.y));

            // Draw image:
            ImGui::Image(textureID, imageSize);
        }
	};
}



#endif // __INCLUDE_GUARD_sceneEditorWindow_h__