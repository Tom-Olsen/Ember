#pragma once
#include "commonGuiFlags.h"
#include "editor.h"
#include "editorSelection.h"
#include "editorWindow.h"
#include "emberTime.h"
#include "eventSystem.h"
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



        // Viewport rect:
        Float2 m_imageTopLeft = Float2::zero;       // top-left of the displayed image in screen-space.
        Float2 m_imageSize = Float2::zero;          // pixel size of the displayed image.
        Float2 m_viewportMousePos = Float2::zero;   // mouse pos relative to image top-left (pixels).
        Float2 m_viewportMousePos01 = Float2::zero; // mouse pos normalized to [0, 1].
        bool m_isMouseInsideViewport = false;



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

            if (windowAspect > textureAspect)
            {
                // Window is wider than texture aspect ratio -> letterboxing:
                m_imageSize.y = windowSize.y;
                m_imageSize.x = m_imageSize.y * textureAspect;
            }
            else
            {
                // Window is taller than texture aspect ratio -> pillarboxing:
                m_imageSize.x = windowSize.x;
                m_imageSize.y = m_imageSize.x / textureAspect;
            }

            // Center the image in the window:
            Float2 cursorPos = Gui::GetCursorPos();
            Float2 offset = 0.5f * (windowSize - m_imageSize);
            Gui::SetCursorPos(cursorPos + offset);

            // Compute viewport-local mouse coordinates from the raw window mouse position:
            m_viewportMousePos = emberCore::EventSystem::MousePos() - Gui::GetCursorScreenPos();
            if (m_imageSize.x > 0.0f && m_imageSize.y > 0.0f)
                m_viewportMousePos01 = m_viewportMousePos / m_imageSize;
            else
                m_viewportMousePos01 = Float2::zero;
            m_isMouseInsideViewport =
                m_viewportMousePos01.x >= 0.0f && m_viewportMousePos01.x <= 1.0f &&
                m_viewportMousePos01.y >= 0.0f && m_viewportMousePos01.y <= 1.0f;

            // Composit render and gizmo textures:
            Gui::Image(renderTextureID, m_imageSize); // draw render texture.
            Gui::SetCursorPos(cursorPos + offset);     // recenter cursor.
            Gui::Image(gizmoTextureID, m_imageSize);  // draw gizmo texture on top (alpha blended).
        }
	};
}