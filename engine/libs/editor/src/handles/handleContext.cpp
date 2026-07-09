#include "handleContext.h"
#include "camera.h"
#include "transform.h"



namespace emberEditor
{
    // Static members:
    // Management:
    bool HandleContext::s_isInitialized;
    // Sizing:
    float HandleContext::s_globalHandleScale;
	float HandleContext::s_screenSizeInPixels;
    // Viewport:
	emberEcs::Camera* HandleContext::s_pCamera;
	Float2 HandleContext::s_viewportSize;
	Float2 HandleContext::s_viewportMousePos;
	Float2 HandleContext::s_viewportMousePos01;
	bool HandleContext::s_viewPortIsHovered;
    // Settings:
	CoordinateSpace HandleContext::s_coordinateSpace;
	bool HandleContext::s_snapEnabled;
	Float3 HandleContext::s_translationSnap;
	float HandleContext::s_rotationSnap;
	Float3 HandleContext::s_scaleSnap;


    
    // Protected methods:
    // Init/Clear:
    void HandleContext::Init()
    {
        if (s_isInitialized)
            return;
        s_isInitialized = true;
        
        // Sizing:
        s_globalHandleScale = 1.5f;
		s_screenSizeInPixels = 100.0f;
        // Viewport:
		s_pCamera = nullptr;
		s_viewportSize = Float2::zero;
		s_viewportMousePos = Float2::zero;
		s_viewportMousePos01 = Float2::zero;
		s_viewPortIsHovered = false;
        // Settings:
		s_coordinateSpace = CoordinateSpace::world;
		s_snapEnabled = false;
		s_translationSnap = 0.25f;
		s_rotationSnap = 15.0f * math::deg2rad;
		s_scaleSnap = 0.1f;
    }
    void HandleContext::Clear()
    {
        s_isInitialized = false;
    }



    // Helpers:
	float HandleContext::ComputeScreenSpaceScale(const Float3& worldPos)
	{
		if (s_pCamera == nullptr)
			return 1.0f;

		float viewportHeight = s_viewportSize.y > 0.0f ? s_viewportSize.y : 1.0f;
		float viewportFraction = s_screenSizeInPixels / viewportHeight;
		if (s_pCamera->GetProjectionType() == emberEcs::Camera::ProjectionType::orthographic)
			return s_pCamera->GetOrthographicSize() * viewportFraction;

		Float3 cameraPos = s_pCamera->GetTransform()->GetPosition();
		float distance = (worldPos - cameraPos).Length();
		return 2.0f * distance * math::Tan(0.5f * s_pCamera->GetFov()) * viewportFraction;
	}



    // Public methods:
    // Setters:
    void HandleContext::SetGlobalHandleScale(float scale)
    {
        s_globalHandleScale = scale;
    }
    void HandleContext::SetCamera(emberEcs::Camera* pCamera)
    {
        s_pCamera = pCamera;
    }
    void HandleContext::SetViewportSize(const Float2& viewportSize)
    {
        s_viewportSize = viewportSize;
    }
    void HandleContext::SetViewportMousePos(const Float2& viewportMousePos)
    {
        s_viewportMousePos = viewportMousePos;
    }
    void HandleContext::SetViewportMousePos01(const Float2& viewportMousePos01)
    {
        s_viewportMousePos01 = viewportMousePos01;
    }
    void HandleContext::SetViewPortIsHovered(bool viewPortIsHovered)
    {
        s_viewPortIsHovered = viewPortIsHovered;
    }
    void HandleContext::SetCoordinateSpace(CoordinateSpace coordinateSpace)
    {
        s_coordinateSpace = coordinateSpace;
    }
    void HandleContext::SetSnap(bool snapEnabled)
    {
        s_snapEnabled = snapEnabled;
    }
    void HandleContext::SetTranslationSnap(float translationSnap)
    {
        s_translationSnap = translationSnap;
    }
    void HandleContext::SetRotationSnap(float rotationSnap)
    {
        s_rotationSnap = rotationSnap;
    }
    void HandleContext::SetScaleSnap(float scaleSnap)
    {
        s_scaleSnap = scaleSnap;
    }



    // Getters:
    float HandleContext::GetGlobalHandleScale()
    {
        return s_globalHandleScale;
    }
    emberEcs::Camera* HandleContext::GetCamera()
    {
        return s_pCamera;
    }
    Float2 HandleContext::GetViewportSize()
    {
        return s_viewportSize;
    }
    Float2 HandleContext::GetViewportMousePos()
    {
        return s_viewportMousePos;
    }
    Float2 HandleContext::GetViewportMousePos01()
    {
        return s_viewportMousePos01;
    }
    bool HandleContext::GetViewPortIsHovered()
    {
        return s_viewPortIsHovered;
    }
    CoordinateSpace HandleContext::GetCoordinateSpace()
    {
        return s_coordinateSpace;
    }
    bool HandleContext::GetSnap()
    {
        return s_snapEnabled;
    }
    float HandleContext::GetTranslationSnap()
    {
        return s_translationSnap;
    }
    float HandleContext::GetRotationSnap()
    {
        return s_rotationSnap;
    }
    float HandleContext::GetScaleSnap()
    {
        return s_scaleSnap;
    }
}