#include "scaleHandle.h"
#include "camera.h"
#include "eventSystem.h"
#include "gizmo.h"
#include "handleContext.h"
#include "material.h"
#include "materialManager.h"
#include "meshGenerator.h"
#include "shaderProperties.h"
#include "transform.h"
#include "transformHandle.h"
#include <optional>



namespace emberEditor
{
	// Static members:
	// Geometry:
	float ScaleHandle::s_axisLength = 1.0f;
	float ScaleHandle::s_axisWidth = 0.02f;
	float ScaleHandle::s_axisCornerCount = 16;
	float ScaleHandle::s_cubeWidth = 0.12f;
	float ScaleHandle::s_minScaleMagnitude = 1e-4f;
    // Interaction:
	float ScaleHandle::s_scaleSensitivity = 0.01f;



	// Public methods:
	// Constructor/Destructor:
	ScaleHandle::ScaleHandle()
	{
		m_handleScale = 1.0f;
		m_pTransform = nullptr;
		m_isDragging = false;
		ResetInteractionState();
		CreateMeshes();
	}
	ScaleHandle::~ScaleHandle()
	{

	}



	// Target:
	void ScaleHandle::SetTarget(emberEcs::Transform* pTransform)
	{
		if (m_pTransform == pTransform)
			return;
		ResetInteractionState();
		m_pTransform = pTransform;
	}
	void ScaleHandle::ClearTarget()
	{
		m_pTransform = nullptr;
		ResetInteractionState();
	}
	bool ScaleHandle::HasTarget() const
	{
		return m_pTransform != nullptr;
	}



	// Handle:
	ScaleHandle::SubHandle ScaleHandle::GetHoveredSubHandle() const
	{
		return m_hoveredSubHandle;
	}
	ScaleHandle::SubHandle ScaleHandle::GetActiveSubHandle() const
	{
		return m_activeSubHandle;
	}
	bool ScaleHandle::GetIsDragging() const
	{
		return m_isDragging;
	}



	// Update/Draw:
	void ScaleHandle::Update()
	{
		if (!HasTarget() || HandleContext::GetCamera() == nullptr)
		{
			ResetInteractionState();
			return;
		}
		UpdateHoveredSubHandle();
		TryBeginDrag();
		UpdateDrag();
	}
	void ScaleHandle::Draw()
	{
		if (!HasTarget())
			return;
		Float4x4 localToWorldMatrix = LocalToWorldMatrix();

		// Draw axis zylinders:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitMaterial"));
		emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisX));
		emberCore::Gizmo::DrawMesh(m_zylinderMesh, localToWorldMatrix * TransformHandle::GetRotationX() * AxisLengthMatrix(ScaleHandle::SubHandle::axisX));
		emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisY));
		emberCore::Gizmo::DrawMesh(m_zylinderMesh, localToWorldMatrix * TransformHandle::GetRotationY() * AxisLengthMatrix(ScaleHandle::SubHandle::axisY));
		emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisZ));
		emberCore::Gizmo::DrawMesh(m_zylinderMesh, localToWorldMatrix * TransformHandle::GetRotationZ() * AxisLengthMatrix(ScaleHandle::SubHandle::axisZ));

		// Draw handle cubes:
        float axisLength = s_axisLength - s_cubeWidth;
		emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisX));
		emberCore::Gizmo::DrawMesh(m_cubeMesh, localToWorldMatrix * TransformHandle::GetRotationX() * Float4x4::Translate(AxisLengthFactor(ScaleHandle::SubHandle::axisX) * axisLength * Float3::up));
		emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisY));
		emberCore::Gizmo::DrawMesh(m_cubeMesh, localToWorldMatrix * TransformHandle::GetRotationY() * Float4x4::Translate(AxisLengthFactor(ScaleHandle::SubHandle::axisY) * axisLength * Float3::up));
		emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisZ));
		emberCore::Gizmo::DrawMesh(m_cubeMesh, localToWorldMatrix * TransformHandle::GetRotationZ() * Float4x4::Translate(AxisLengthFactor(ScaleHandle::SubHandle::axisZ) * axisLength * Float3::up));
		emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::center));
		emberCore::Gizmo::DrawMesh(m_cubeMesh, localToWorldMatrix);
		emberCore::Gizmo::ResetMaterial();

		// Visualize scale interaction regions:
		//emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitMaterial"));
		//emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisX) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * TransformHandle::GetRotationX());
		//emberCore::Gizmo::DrawMesh(m_sphereMesh, localToWorldMatrix * TransformHandle::GetRotationX() * cubeTranslation);
		//emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisY) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * TransformHandle::GetRotationY());
		//emberCore::Gizmo::DrawMesh(m_sphereMesh, localToWorldMatrix * TransformHandle::GetRotationY() * cubeTranslation);
		//emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::axisZ) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * TransformHandle::GetRotationZ());
		//emberCore::Gizmo::DrawMesh(m_sphereMesh, localToWorldMatrix * TransformHandle::GetRotationZ() * cubeTranslation);
		//emberCore::Gizmo::SetColor(SubHandleStateColor(ScaleHandle::SubHandle::center) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_sphereMesh, localToWorldMatrix);
		//emberCore::Gizmo::ResetMaterial();
	}



	// Private methods:
	void ScaleHandle::ResetInteractionState()
	{
		if (m_isDragging)
			emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);

		m_isDragging = false;
		m_hoveredSubHandle = ScaleHandle::SubHandle::none;
		m_activeSubHandle = ScaleHandle::SubHandle::none;
		m_dragStartPosition = Float3::zero;
		m_dragStartScale = Float3::one;
		m_dragStartHitPoint = Float3::zero;
		m_dragAxisDir = Float3::zero;
		m_dragPlaneNormal = Float3::zero;
		m_dragStartMousePos = Float2::zero;
		m_dragStartHandleSize = 1.0f;
		m_grabAxisFraction = 1.0f;
		m_dragAxisLengthFactor = 1.0f;
	}



	// Mesh generation:
	void ScaleHandle::CreateMeshes()
	{
        float axisLength = s_axisLength - s_cubeWidth;
		m_zylinderMesh = emberCore::MeshGenerator::ZylinderMantleSmooth( s_axisWidth, axisLength, (int)s_axisCornerCount, "scaleHandleZylinder").Translate(0.5f * axisLength * Float3::up);
		m_capsuleMesh = emberCore::MeshGenerator::Capsule( 2.0f * s_axisWidth, axisLength, 8, "scaleHandleCapsule").Translate(0.5f * axisLength * Float3::up);
		m_cubeMesh = emberCore::MeshGenerator::Cube().Scale(s_cubeWidth);
        float cubeRadius = 0.5f * math::sqrt3 * s_cubeWidth;
		m_sphereMesh = emberCore::MeshGenerator::CubeSphere(cubeRadius, 8, "scaleHandleSphere");
	}

	// Interaction:
	void ScaleHandle::TryBeginDrag()
	{
		// Return if no drag:
		if (m_isDragging || m_hoveredSubHandle == ScaleHandle::SubHandle::none)
			return;
		if (!emberCore::EventSystem::MouseDown(emberCommon::Input::MouseButton::Left))
			return;
		if (!emberCore::EventSystem::TryLockMouseButton(emberCommon::Input::MouseButton::Left))
			return;

		// Begin drag:
		m_activeSubHandle = m_hoveredSubHandle;
		m_dragStartPosition = m_pTransform->GetPosition();
		m_dragStartScale = m_pTransform->GetScale();
		m_dragStartMousePos = HandleContext::GetViewportMousePos();
		m_dragStartHandleSize = Size();

		// Find drag plane for axis scaling:
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		if (IsAxisSubHandle(m_activeSubHandle))
		{
			m_dragAxisDir = Float3(HandleRotationMatrix() * Float4(SubHandleDirection(m_activeSubHandle), 0.0f));
			m_dragPlaneNormal = ray.direction - Float3::Dot(ray.direction, m_dragAxisDir) * m_dragAxisDir;
			if (m_dragPlaneNormal.IsEpsilonZero())
			{
				m_activeSubHandle = ScaleHandle::SubHandle::none;
				emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);
				return;
			}
			m_dragPlaneNormal = m_dragPlaneNormal.Normalize();

			// Find initial hit point on drag plane:
			std::optional<Float3> hit = ray.HitOnPlane(m_dragStartPosition, m_dragPlaneNormal);
			if (!hit.has_value())
			{
				m_activeSubHandle = ScaleHandle::SubHandle::none;
				emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);
				return;
			}
			m_dragStartHitPoint = hit.value();

			// Resolve initial grab fraction:
			float axisLength = m_dragStartHandleSize * (s_axisLength - s_cubeWidth);
			float hitDistance = Float3::Dot(m_dragStartHitPoint - m_dragStartPosition, m_dragAxisDir);
			m_grabAxisFraction = hitDistance / axisLength;
			if (math::Abs(m_grabAxisFraction) < s_minScaleMagnitude)
			{
				m_activeSubHandle = ScaleHandle::SubHandle::none;
				emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);
				return;
			}
		}

		// Start drag:
		m_isDragging = true;
		emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
	}
	void ScaleHandle::UpdateDrag()
	{
		if (!m_isDragging)
			return;

		// Cancel drag:
		if (emberCore::EventSystem::MouseUp(emberCommon::Input::MouseButton::Left))
		{
		    emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
		    ResetInteractionState();
			return;
		}

		// Update entity scale:
		if (IsCenterSubHandle(m_activeSubHandle))
		{
			// Center scaling uses mouse delta:
			Float2 mouseDelta = HandleContext::GetViewportMousePos() - m_dragStartMousePos;
			float delta = s_scaleSensitivity * (mouseDelta.x - mouseDelta.y);
			SetScale(m_activeSubHandle, delta);
		}
		else // Axis scaling:
		{
			Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
			std::optional<Float3> hit = ray.HitOnPlane(m_dragStartPosition, m_dragPlaneNormal);
			if (hit.has_value())
			{
				// Resolve axis scale factor:
				float axisLength = m_dragStartHandleSize * (s_axisLength - s_cubeWidth);
				float hitDistance = Float3::Dot(hit.value() - m_dragStartPosition, m_dragAxisDir);
				m_dragAxisLengthFactor = hitDistance / (m_grabAxisFraction * axisLength);
				SetScale(m_activeSubHandle, m_dragAxisLengthFactor);
			}
		}
		emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
	}
	void ScaleHandle::UpdateHoveredSubHandle()
	{
		if (m_isDragging)
		{
			m_hoveredSubHandle = m_activeSubHandle;
			return;
		}

		m_hoveredSubHandle = ScaleHandle::SubHandle::none;
		if (!HandleContext::GetViewPortIsHovered())
			return;

		// Raycast setup:
		Float4x4 localToWorldMatrix = LocalToWorldMatrix();
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		float closestHitDistanceSq = math::maxValue;

		// Check each handle:
		TryPickAxisSubHandle(ScaleHandle::SubHandle::axisX, localToWorldMatrix * TransformHandle::GetRotationX(), ray, closestHitDistanceSq);
		TryPickAxisSubHandle(ScaleHandle::SubHandle::axisY, localToWorldMatrix * TransformHandle::GetRotationY(), ray, closestHitDistanceSq);
		TryPickAxisSubHandle(ScaleHandle::SubHandle::axisZ, localToWorldMatrix * TransformHandle::GetRotationZ(), ray, closestHitDistanceSq);
		TryPickCubeSubHandle(ScaleHandle::SubHandle::center, m_pTransform->GetPosition(), ray, closestHitDistanceSq);
	}



	// Helpers:
	float ScaleHandle::Size() const
	{
		if (!HasTarget())
			return 1.0f;
		return SizeAtPosition(m_pTransform->GetPosition());
	}
	float ScaleHandle::SizeAtPosition(const Float3& position) const
	{
		return m_handleScale * HandleContext::GetGlobalHandleScale() * HandleContext::ComputeScreenSpaceScale(position);
	}
	Float4x4 ScaleHandle::HandleRotationMatrix() const
	{
        // Scaling handle only support local space.
		if (!HasTarget())
			return Float4x4::identity;
		return m_pTransform->GetRotation4x4();
	}
	Float4x4 ScaleHandle::LocalToWorldMatrix()
	{
		if (!HasTarget())
			return Float4x4::identity;
		return Float4x4::TRS(m_pTransform->GetPosition(), HandleRotationMatrix(), Size());
	}
	Float4 ScaleHandle::SubHandleStateColor(ScaleHandle::SubHandle subHandle)
	{
		if (m_activeSubHandle == subHandle)
			return TransformHandle::GetActiveColor();
		if (m_hoveredSubHandle == subHandle)
			return TransformHandle::GetHoverColor();
		return SubHandleBaseColor(subHandle);
	}
	float ScaleHandle::AxisLengthFactor(ScaleHandle::SubHandle subHandle) const
	{
		if (m_isDragging && m_activeSubHandle == subHandle && IsAxisSubHandle(subHandle))
			return m_dragAxisLengthFactor;
		return 1.0f;
	}
	Float4x4 ScaleHandle::AxisLengthMatrix(ScaleHandle::SubHandle subHandle) const
	{
		// AxisLengthScale:
		float factor = AxisLengthFactor(subHandle);
		Float4x4 scale = Float4x4::Scale(Float3(1.0f, 1.0f, math::Abs(factor)));
		if (factor < 0.0f)
			return Float4x4::RotateY(math::pi) * scale;
		return scale;
	}
	void ScaleHandle::SetScale(ScaleHandle::SubHandle subHandle, float amount)
	{
		// ApplyLocalScaleChange:
		Float3 scale = m_dragStartScale;
		if (IsCenterSubHandle(subHandle))
			scale += amount * Float3::one;
		else if (subHandle == ScaleHandle::SubHandle::axisX)
			scale.x *= amount;
		else if (subHandle == ScaleHandle::SubHandle::axisY)
			scale.y *= amount;
		else if (subHandle == ScaleHandle::SubHandle::axisZ)
			scale.z *= amount;

        // Prevent zero scale:
		for (int i = 0; i < Float3::size; i++)
		{
			if (math::Abs(scale[i]) < s_minScaleMagnitude)
				scale[i] = scale[i] < 0.0f ? -s_minScaleMagnitude : s_minScaleMagnitude;
		}
		m_pTransform->SetScale(scale);
	}
	void ScaleHandle::TryPickAxisSubHandle(ScaleHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq)
	{
		// Construct interaction capsule:
		float radius = 2.0f * s_axisWidth;
        float axisLength = s_axisLength - s_cubeWidth;
		Float3 point0 = Float3(localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
		Float3 point1 = Float3(localToWorldMatrix * Float4(0.0f, 0.0f, axisLength, 1.0f));
		Capsule capsule = Capsule(point0, point1, radius * Size());

		// Ray-capsule hit:
		std::optional<Float3> hit = capsule.IntersectRay(ray);
		if (hit.has_value())
		{
		    // Check if new hit is closer:
			float hitDistanceSq = Float3::DistanceSq(ray.origin, hit.value());
			if (hitDistanceSq < closestHitDistanceSq)
			{
				closestHitDistanceSq = hitDistanceSq;
				m_hoveredSubHandle = subHandle;
			}
		}
		TryPickCubeSubHandle(subHandle, point1, ray, closestHitDistanceSq);
	}
	void ScaleHandle::TryPickCubeSubHandle(ScaleHandle::SubHandle subHandle, const Float3& cubeWorldPosition, const Ray& ray, float& closestHitDistanceSq)
	{
		// Ray-sphere hit:
        float cubeRadius = 0.5f * math::sqrt3 * s_cubeWidth;
		Sphere sphere = Sphere(cubeWorldPosition, cubeRadius * Size());
		std::optional<Float3> hit = sphere.IntersectRay(ray);
		if (!hit.has_value())
			return;

		// Check if new hit is closer:
		float hitDistanceSq = Float3::DistanceSq(ray.origin, hit.value());
		if (hitDistanceSq < closestHitDistanceSq)
		{
			closestHitDistanceSq = hitDistanceSq;
			m_hoveredSubHandle = subHandle;
		}
	}



	// Static helpers:
	Float3 ScaleHandle::SubHandleDirection(ScaleHandle::SubHandle subHandle)
	{
		switch (subHandle)
		{
			case ScaleHandle::SubHandle::axisX: return Float3::right;
			case ScaleHandle::SubHandle::axisY: return Float3::forward;
			case ScaleHandle::SubHandle::axisZ: return Float3::up;
			default: return Float3::zero;
		}
	}
	Float4 ScaleHandle::SubHandleBaseColor(ScaleHandle::SubHandle subHandle)
	{
		switch (subHandle)
		{
			case ScaleHandle::SubHandle::axisX: return TransformHandle::GetColorX();
			case ScaleHandle::SubHandle::axisY: return TransformHandle::GetColorY();
			case ScaleHandle::SubHandle::axisZ: return TransformHandle::GetColorZ();
			case ScaleHandle::SubHandle::center: return TransformHandle::GetColorCenter();
			default: return Float4::zero;
		}
	}
	bool ScaleHandle::IsAxisSubHandle(ScaleHandle::SubHandle subHandle)
	{
		return subHandle == ScaleHandle::SubHandle::axisX
			|| subHandle == ScaleHandle::SubHandle::axisY
			|| subHandle == ScaleHandle::SubHandle::axisZ;
	}
	bool ScaleHandle::IsCenterSubHandle(ScaleHandle::SubHandle subHandle)
	{
		return subHandle == ScaleHandle::SubHandle::center;
	}
}