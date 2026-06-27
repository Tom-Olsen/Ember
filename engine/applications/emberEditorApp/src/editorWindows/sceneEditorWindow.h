#pragma once
#include "editorWindow.h"
#include "gizmoContext.h"
#include "transformGizmo.h"



namespace emberEditor
{
	class SceneEditorWindow : public emberCore::EditorWindow
	{
    private: // Members:
        // Viewport rect:
        Float2 m_imageTopLeft = Float2::zero;       // top-left of the displayed image in screen-space.
        Float2 m_imageSize = Float2::zero;          // pixel size of the displayed image.
        Float2 m_viewportMousePos = Float2::zero;   // mouse pos relative to image top-left (pixels).
        Float2 m_viewportMousePos01 = Float2::zero; // mouse pos normalized to [0, 1].
        bool m_isMouseInsideViewport = false;
        GizmoContext m_gizmoContext;
        TransformGizmo m_transformGizmo;

    public: // Methods:
        // Constructor:
		SceneEditorWindow();

        // Overrides:
        void PreRender() override;
        void Render() override;
	};
}