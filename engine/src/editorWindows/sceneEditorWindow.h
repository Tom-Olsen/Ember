#ifndef __INCLUDE_GUARD_sceneEditorWindow_h__
#define __INCLUDE_GUARD_sceneEditorWindow_h__
#include "dearImGui.h"
#include "editor.h"
#include "editorWindow.h"
#include "emberTime.h"
#include "renderPassManager.h"
#include "renderTexture2d.h"
#include "vulkanForwardRenderPass.h"



namespace emberEngine
{
	struct SceneEditorWindow : EditorWindow
	{
		SceneEditorWindow()
		{
            m_name = "Scene";
            m_ID = 0;
            m_windowFlags = WindowFlag::NoCollapse | WindowFlag::NoScrollbar | WindowFlag::NoScrollWithMouse;
            m_wantCaptureEvents = false;
            m_nameID = m_name + "##" + std::to_string(m_ID);
		}

        void Render() override
        {
            // Prepare render texture:
            Texture2d* pRenderTexture = (Texture2d*)RenderPassManager::GetForwardRenderPass()->GetRenderTexture();
            uintptr_t textureID = DearImGui::GetTextureID(pRenderTexture);

            // Get imgui window:
            const Float2 windowSize = DearImGui::GetContentRegionalAvail();

            // Texture and window aspect ratios:
            const float textureAspect = (float)pRenderTexture->GetWidth() / (float)pRenderTexture->GetHeight();
            const float windowAspect = windowSize.x / windowSize.y;

            Float2 imageSize;
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
            Float2 cursorPos = DearImGui::GetCursorPos();
            Float2 offset = 0.5f * (windowSize - imageSize);
            DearImGui::SetCursorPos(cursorPos + offset);

            // Draw image:
            DearImGui::Image(textureID, imageSize);
        }
	};
}



#endif // __INCLUDE_GUARD_sceneEditorWindow_h__