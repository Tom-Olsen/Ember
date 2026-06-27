#include "sceneEditorWindow.h"
#include "commonGuiFlags.h"
#include "editor.h"
#include "editorSelection.h"
#include "emberTime.h"
#include "eventSystem.h"
#include "gui.h"
#include "renderer.h"
#include "scene.h"
#include "texture2d.h"
#include "transform.h"



namespace emberEditor
{
    // Easy access to emberEngine Gui:
    using Gui = emberCore::Gui;


    
    // Public methods:
    // Constructor:
	SceneEditorWindow::SceneEditorWindow()
	{
        m_name = "Scene";
        m_ID = 0;
        m_windowFlags = emberCommon::GuiWindowFlags::noCollapse | emberCommon::GuiWindowFlags::noScrollbar | emberCommon::GuiWindowFlags::noScrollWithMouse;
        m_wantCaptureEvents = false;
        m_nameID = m_name + "##" + std::to_string(m_ID);
	}

    // Overrides:
    void SceneEditorWindow::PreRender()
    {
        // Scene window closed:
        if (!m_isOpen)
        {
            m_transformGizmo.ClearTarget();
            return;
        }

        // Update gizmo context:
        emberEcs::Scene* pScene = emberEcs::Scene::GetActiveScene();
        m_gizmoContext.pCamera = pScene == nullptr ? nullptr : pScene->GetActiveCamera();
        m_gizmoContext.viewportSize = m_imageSize;
        m_gizmoContext.viewportMousePos = m_viewportMousePos;
        m_gizmoContext.viewportMousePos01 = m_viewportMousePos01;
        m_gizmoContext.isHovered = m_isOpen && m_isHovered && m_isMouseInsideViewport;

        // Sync transform gizmo target:
        if (!EditorSelection::HasSelectedEntity())
        {
            m_transformGizmo.ClearTarget();
            return;
        }
        emberEcs::Entity selected = EditorSelection::GetSelectedEntity();
        m_transformGizmo.SetTarget(selected.GetTransform());
        
        // Update and draw transform gizmo:
        m_transformGizmo.Update(m_gizmoContext);
        m_transformGizmo.Draw(m_gizmoContext);
    }
    void SceneEditorWindow::Render()
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
        m_imageTopLeft = Gui::GetCursorScreenPos();

        // Compute viewport-local mouse coordinates from the raw window mouse position:
        m_viewportMousePos = emberCore::EventSystem::MousePos() - m_imageTopLeft;
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
}