#pragma once
#include "commonGuiFlags.h"
#include "editor.h"
#include "editorSelection.h"
#include "editorWindow.h"
#include "emberTime.h"
#include "gizmo.h"
#include "gui.h"
#include "renderer.h"
#include "texture2d.h"
#include "transform.h"



namespace emberEditor
{
	struct SceneEditorWindow : public emberCore::EditorWindow
	{
        // Easy access to emberEngine Gui:
        using Gui = emberCore::Gui;


        // Constructor:
		SceneEditorWindow()
		{
            m_name = "Scene";
            m_ID = 0;
            m_windowFlags = emberCommon::GuiWindowFlags::noCollapse | emberCommon::GuiWindowFlags::noScrollbar | emberCommon::GuiWindowFlags::noScrollWithMouse;
            m_wantCaptureEvents = false;
            m_nameID = m_name + "##" + std::to_string(m_ID);
		}



        // Overrides:
        void PreRender() override
        {
            if (!EditorSelection::HasSelectedEntity())
                return;

            emberEcs::Entity selected = EditorSelection::GetSelectedEntity();
            emberEcs::Transform* pTransform = selected.GetTransform();
            if (pTransform == nullptr)
                return;

            Float3 pos = pTransform->GetPosition();
            emberCore::Gizmo::SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
            emberCore::Gizmo::DrawSphere(Float4x4::TRS(pos, Float3x3::identity, Float3(0.1f)));
        }
        void Render() override
        {
            // Prepare render texture:
            emberCore::Texture2d renderTexture = emberCore::Renderer::GetRenderTexture();
            uintptr_t renderTextureID = Gui::GetTextureID(renderTexture);
            emberCore::Texture2d gizmoTexture = emberCore::Renderer::GetGizmoTexture();
            uintptr_t gizmoTextureID = Gui::GetTextureID(gizmoTexture);

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

            // Composit render and gizmo textures:
            Gui::Image(renderTextureID, imageSize); // draw render texture.
            Gui::SetCursorPos(cursorPos + offset);   // recenter cursor.
            Gui::Image(gizmoTextureID, imageSize);  // draw gizmo texture on top (alpha blended).
        }
	};
}