#pragma once
#include "commonGuiFlags.h"
#include "editor.h"
#include "editorWindow.h"
#include "emberTime.h"
#include "gui.h"
#include "renderer.h"
#include "texture2d.h"



namespace emberEditor
{
	struct SceneEditorWindow : public emberEngine::EditorWindow
	{
        // Easy access to emberEngine Gui:
        using Gui = emberEngine::Gui;



		SceneEditorWindow()
		{
            m_name = "Scene";
            m_ID = 0;
            m_windowFlags = emberCommon::GuiWindowFlag_NoCollapse | emberCommon::GuiWindowFlag_NoScrollbar | emberCommon::GuiWindowFlag_NoScrollWithMouse;
            m_wantCaptureEvents = false;
            m_nameID = m_name + "##" + std::to_string(m_ID);
		}



        void Render() override
        {
            // Prepare render texture:
            emberEngine::Texture2d renderTexture = emberEngine::Renderer::GetRenderTexture();
            uintptr_t textureID = Gui::GetTextureID(renderTexture);

            // Get imgui window:
            const Float2 windowSize = Gui::GetContentRegionAvail();

            // Texture and window aspect ratios:
            const float textureAspect = (float)renderTexture.GetWidth() / (float)renderTexture.GetHeight();
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
            Float2 cursorPos = Gui::GetCursorPos();
            Float2 offset = 0.5f * (windowSize - imageSize);
            Gui::SetCursorPos(cursorPos + offset);

            // Draw image:
            Gui::Image(textureID, imageSize);
        }
	};
}