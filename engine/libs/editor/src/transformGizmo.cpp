#include "transformGizmo.h"
#include "camera.h"
#include "gizmo.h"
#include "transform.h"



namespace emberEditor
{
	// Public methods:
	void TransformGizmo::SetTarget(emberEcs::Transform* pTransform)
	{
		if (m_pTransform == pTransform)
			return;
		m_pTransform = pTransform;
		ResetInteractionState();
	}
	void TransformGizmo::ClearTarget()
	{
		m_pTransform = nullptr;
		ResetInteractionState();
	}
	bool TransformGizmo::HasTarget() const
	{
		return m_pTransform != nullptr;
	}

	void TransformGizmo::SetMode(TransformGizmo::Mode mode)
	{
		if (m_mode == mode)
			return;
		m_mode = mode;
		ResetInteractionState();
	}
	TransformGizmo::Mode TransformGizmo::GetMode() const
	{
		return m_mode;
	}
	TransformGizmo::Handle TransformGizmo::GetHoveredHandle() const
	{
		return m_hoveredHandle;
	}
	TransformGizmo::Handle TransformGizmo::GetActiveHandle() const
	{
		return m_activeHandle;
	}
	bool TransformGizmo::IsDragging() const
	{
		return m_isDragging;
	}

	void TransformGizmo::Update(const GizmoContext& context)
	{
		if (!HasTarget() || context.pCamera == nullptr)
		{
			ResetInteractionState();
			return;
		}

		switch (m_mode)
		{
			case TransformGizmo::Mode::translate:
				UpdateTranslate(context);
				break;
			case TransformGizmo::Mode::rotate:
				UpdateRotate(context);
				break;
			case TransformGizmo::Mode::scale:
				UpdateScale(context);
				break;
			default:
				break;
		}
	}
	void TransformGizmo::Draw(const GizmoContext& context)
	{
		if (!HasTarget() || context.pCamera == nullptr)
			return;

		switch (m_mode)
		{
			case TransformGizmo::Mode::translate:
				DrawTranslate(context);
				break;
			case TransformGizmo::Mode::rotate:
				DrawRotate(context);
				break;
			case TransformGizmo::Mode::scale:
				DrawScale(context);
				break;
			default:
				break;
		}
	}



	// Private methods:
	void TransformGizmo::ResetInteractionState()
	{
		m_hoveredHandle = TransformGizmo::Handle::none;
		m_activeHandle = TransformGizmo::Handle::none;
        // Drag state:
		m_isDragging = false;
		//m_dragStartPosition = Float3::zero;
		//m_dragStartRotation = Float3x3::identity;
		//m_dragStartScale = Float3::one;
		//m_dragPlaneNormal = Float3::zero;
		//m_dragStartHitPoint = Float3::zero;
		//m_dragAxisDir = Float3::zero;
		//m_dragGizmoSize = 1.0f;
		//m_dragRingPerp1 = Float3::zero;
		//m_dragRingPerp2 = Float3::zero;
		//m_dragStartAngle = 0.0f;
	}
    // Updates:
	void TransformGizmo::UpdateTranslate(const GizmoContext& context)
	{
		(void)context;
		m_hoveredHandle = TransformGizmo::Handle::none;
	}
	void TransformGizmo::UpdateRotate(const GizmoContext& context)
	{
		(void)context;
		m_hoveredHandle = TransformGizmo::Handle::none;
	}
	void TransformGizmo::UpdateScale(const GizmoContext& context)
	{
		(void)context;
		m_hoveredHandle = TransformGizmo::Handle::none;
	}
    // Draws:
	void TransformGizmo::DrawTranslate(const GizmoContext& context)
	{
		Float3 position = m_pTransform->GetPosition();
		float size = 0.3f * ComputeScreenSpaceScale(context, position);
		emberCore::Gizmo::SetColor(Float4::red);
		emberCore::Gizmo::DrawArrow(position, Float3::right, size);
		emberCore::Gizmo::SetColor(Float4::green);
		emberCore::Gizmo::DrawArrow(position, Float3::forward, size);
		emberCore::Gizmo::SetColor(Float4::blue);
		emberCore::Gizmo::DrawArrow(position, Float3::up, size);

	}
	void TransformGizmo::DrawRotate(const GizmoContext& context)
	{
		Float3 position = m_pTransform->GetPosition();
		float markerSize = 0.1f * ComputeScreenSpaceScale(context, position);
		emberCore::Gizmo::SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
		emberCore::Gizmo::DrawSphere(Float4x4::TRS(position, Float3x3::identity, Float3(markerSize)));
	}
	void TransformGizmo::DrawScale(const GizmoContext& context)
	{
		Float3 position = m_pTransform->GetPosition();
		float markerSize = 0.1f * ComputeScreenSpaceScale(context, position);
		emberCore::Gizmo::SetColor(Float4(1.0f, 1.0f, 0.0f, 1.0f));
		emberCore::Gizmo::DrawSphere(Float4x4::TRS(position, Float3x3::identity, Float3(markerSize)));
	}
    // Helpers:
	float TransformGizmo::ComputeScreenSpaceScale(const GizmoContext& context, const Float3& worldPos) const
	{
		if (context.pCamera == nullptr)
			return 1.0f;

		float viewportHeight = context.viewportSize.y > 0.0f ? context.viewportSize.y : 1.0f;
		float viewportFraction = s_screenSizePixels / viewportHeight;
		if (context.pCamera->GetProjectionType() == emberEcs::Camera::ProjectionType::orthographic)
			return context.pCamera->GetOrthographicSize() * viewportFraction;

		Float3 cameraPos = context.pCamera->GetTransform()->GetPosition();
		float distance = (worldPos - cameraPos).Length();
		return 2.0f * distance * math::Tan(0.5f * context.pCamera->GetFov()) * viewportFraction;
	}
}