#pragma once
#include "emberMath.h"



// Forward declarations:
namespace emberEcs
{
	class Camera;
}



namespace emberEditor
{
    class HandleContext
    {
	private: // Members:
        // Management:
        static bool s_isInitialized;
        // Sizing:
        static float s_globalHandleScale;
		static float s_screenSizeInPixels;
        // Viewport:
		static emberEcs::Camera* s_pCamera;
		static Float2 s_viewportSize;
		static Float2 s_viewportMousePos;
		static Float2 s_viewportMousePos01;
		static bool s_viewPortIsHovered;
        // Settings:
		static CoordinateSpace s_coordinateSpace;
		static bool s_snapEnabled;
		static float s_translationSnap;
		static float s_rotationSnap;
		static float s_scaleSnap;

    public: // Methods:
        // Init/Clear:
        static void Init();
        static void Clear();

        // Setters:
        static void SetGlobalHandleScale(float scale);
        static void SetCamera(emberEcs::Camera* pCamera);
        static void SetViewportSize(const Float2& viewportSize);
        static void SetViewportMousePos(const Float2& viewportMousePos);
        static void SetViewportMousePos01(const Float2& viewportMousePos01);
        static void SetViewPortIsHovered(bool viewPortIsHovered);
        static void SetCoordinateSpace(CoordinateSpace coordinateSpace);
        static void SetSnap(bool snapEnabled);
        static void SetTranslationSnap(float translationSnap);
        static void SetRotationSnap(float rotationSnap);
        static void SetScaleSnap(float scaleSnap);

        // Getters:
        static float GetGlobalHandleScale();
        static emberEcs::Camera* GetCamera();
        static Float2 GetViewportSize();
        static Float2 GetViewportMousePos();
        static Float2 GetViewportMousePos01();
        static bool GetViewPortIsHovered();
        static CoordinateSpace GetCoordinateSpace();
        static bool GetSnap();
        static float GetTranslationSnap();
        static float GetRotationSnap();
        static float GetScaleSnap();

        // Helpers:
		static float ComputeScreenSpaceScale(const Float3& worldPos);
		static float ApplyTranslationSnap(float translation);
		static float ApplyRotationSnap(float angleRadians);
		static float ApplyScaleSnap(float scale);

    private: // Methods:
		static bool SnapIsActive();
		static float SnapToIncrement(float value, float snapIncrement);

        // Delete all constructors:
        HandleContext() = delete;
        HandleContext(const HandleContext&) = delete;
        HandleContext& operator=(const HandleContext&) = delete;
        HandleContext(HandleContext&&) = delete;
        HandleContext& operator=(HandleContext&&) = delete;
        ~HandleContext() = delete;
    };
}