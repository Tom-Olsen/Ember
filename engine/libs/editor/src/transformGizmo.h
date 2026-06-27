#pragma once
#include "emberMath.h"
#include "gizmoContext.h"



// Forward declarations:
namespace emberEcs
{
	class Transform;
}



namespace emberEditor
{
	class TransformGizmo
	{
    public: // Enums:
	    enum class Mode
	    {
	    	translate,
	    	rotate,
	    	scale
	    };

	    enum class Handle
	    {
	    	none,
	    	axisX,
	    	axisY,
	    	axisZ,
	    	planeXY,
	    	planeYZ,
	    	planeXZ,
	    	uniform,
	    	rotX,
	    	rotY,
	    	rotZ
	    };

	private: // Members:
		emberEcs::Transform* m_pTransform = nullptr;
		Mode m_mode = Mode::translate;
		Handle m_hoveredHandle = Handle::none;
		Handle m_activeHandle = Handle::none;
        
		// Drag state: (currently unused)
		bool m_isDragging = false;
        //Float3 m_dragStartPosition = Float3::zero;
		//Float3x3 m_dragStartRotation = Float3x3::identity;
		//Float3 m_dragStartScale = Float3::one;
		//Float3 m_dragPlaneNormal = Float3::zero;
		//Float3 m_dragStartHitPoint = Float3::zero;
		//Float3 m_dragAxisDir = Float3::zero;
		//float m_dragGizmoSize = 1.0f;
		//Float3 m_dragRingPerp1 = Float3::zero;
		//Float3 m_dragRingPerp2 = Float3::zero;
		//float m_dragStartAngle = 0.0f;

		// Sizing / picking constants:
		static constexpr float s_screenSizePixels = 200.0f;

	public: // Methods:
		void SetTarget(emberEcs::Transform* pTransform);
		void ClearTarget();
		bool HasTarget() const;

		void SetMode(Mode mode);
		Mode GetMode() const;
		Handle GetHoveredHandle() const;
		Handle GetActiveHandle() const;
		bool IsDragging() const;

		void Update(const GizmoContext& context);
		void Draw(const GizmoContext& context);

	private: // Methods:
        void ResetInteractionState();
        // Updates:
		void UpdateTranslate(const GizmoContext& context);
		void UpdateRotate(const GizmoContext& context);
		void UpdateScale(const GizmoContext& context);
        // Draws:
		void DrawTranslate(const GizmoContext& context);
		void DrawRotate(const GizmoContext& context);
		void DrawScale(const GizmoContext& context);
        // Helpers:
		float ComputeScreenSpaceScale(const GizmoContext& context, const Float3& worldPos) const;
	};
}