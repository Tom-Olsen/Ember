#include "rotateHandle.h"
#include "camera.h"
#include "eventSystem.h"
#include "gizmo.h"
#include "handleContext.h"
#include "material.h"
#include "materialManager.h"
#include "meshGenerator.h"
#include "shaderProperties.h"
#include "transform.h"
#include <optional>



namespace emberEditor
{
	// Static members:
    // Handle colors:
	Float4 RotateHandle::s_colorX = Float4(1.0f, 0.38039, 0.24314, 1.0f);
	Float4 RotateHandle::s_colorY = Float4(0.78039f, 1.0f, 0.41569, 1.0f);
	Float4 RotateHandle::s_colorZ = Float4(0.31765f, 0.66667f, 1.0f, 1.0f);
	Float4 RotateHandle::s_hoverColor = Float4::yellow;
	Float4 RotateHandle::s_activeColor = Float4::white;
    // Rotation matrizes:
	Float4x4 RotateHandle::s_rotX = Float4x4::RotateFromTo(Float3::up, Float3::right) * Float4x4::RotateZ(math::pi2);
	Float4x4 RotateHandle::s_rotY = Float4x4::RotateFromTo(Float3::up, Float3::forward) * Float4x4::RotateZ(-math::pi2);
	Float4x4 RotateHandle::s_rotZ = Float4x4::identity;
    // Geometry:
	float RotateHandle::s_arcStart = 0.8f;
	float RotateHandle::s_arcEnd = 1.0f;
	float RotateHandle::s_arcWidth = s_arcEnd - s_arcStart;
	float RotateHandle::s_arcCornerCount = 32;
    // Interaction:
	float RotateHandle::s_rotationSensitivity = 0.01f;



	// Public methods:
	// Constructor/Destructor:
	RotateHandle::RotateHandle()
	{
		m_handleScale = 1.0f;
		m_coordinateSpace = CoordinateSpace::world;
		m_pTransform = nullptr;
		m_isDragging = false;
		m_octantIndex = 0;
		ResetInteractionState();
		CreateMeshes();
	}
	RotateHandle::~RotateHandle()
	{

	}



	// Target:
	void RotateHandle::SetTarget(emberEcs::Transform* pTransform)
	{
		if (m_pTransform == pTransform)
			return;
		ResetInteractionState();
		m_pTransform = pTransform;
	}
	void RotateHandle::ClearTarget()
	{
		m_pTransform = nullptr;
		ResetInteractionState();
	}
	bool RotateHandle::HasTarget() const
	{
		return m_pTransform != nullptr;
	}



	// Coordinate space:
	void RotateHandle::SetCoordinateSpace(CoordinateSpace coordinateSpace)
	{
		m_coordinateSpace = coordinateSpace;
	}
	CoordinateSpace RotateHandle::GetCoordinateSpace() const
	{
		return m_coordinateSpace;
	}



	// Handle:
	RotateHandle::SubHandle RotateHandle::GetHoveredSubHandle() const
	{
		return m_hoveredSubHandle;
	}
	RotateHandle::SubHandle RotateHandle::GetActiveSubHandle() const
	{
		return m_activeSubHandle;
	}
	bool RotateHandle::GetIsDragging() const
	{
		return m_isDragging;
	}



	// Update/Draw:
	void RotateHandle::Update()
	{
		if (!HasTarget() || HandleContext::GetCamera() == nullptr)
		{
			ResetInteractionState();
			return;
		}
		UpdateOctant();
		UpdateHoveredSubHandle();
		TryBeginDrag();
		UpdateDrag();
	}
	void RotateHandle::Draw()
	{
		if (!HasTarget())
			return;
		Float4x4 localToWorldMatrix = LocalToWorldMatrix();

        // Draw arcs:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoUnlitMaterial"));
		emberCore::Gizmo::SetColor(SubHandleStateColor(RotateHandle::SubHandle::axisX));
		emberCore::Gizmo::SetCullMode(emberCommon::CullMode::none);
        emberCore::Gizmo::DrawMesh(m_arcMesh, localToWorldMatrix * OctantMatrix(RotateHandle::SubHandle::axisX, m_octantIndex) * s_rotX);
		emberCore::Gizmo::SetColor(SubHandleStateColor(RotateHandle::SubHandle::axisY));
        emberCore::Gizmo::DrawMesh(m_arcMesh, localToWorldMatrix * OctantMatrix(RotateHandle::SubHandle::axisY, m_octantIndex) * s_rotY);
		emberCore::Gizmo::SetColor(SubHandleStateColor(RotateHandle::SubHandle::axisZ));
        emberCore::Gizmo::DrawMesh(m_arcMesh, localToWorldMatrix * OctantMatrix(RotateHandle::SubHandle::axisZ, m_octantIndex) * s_rotZ);
		emberCore::Gizmo::ResetCullMode();
        emberCore::Gizmo::ResetMaterial();
    }



	// Private methods:
	void RotateHandle::ResetInteractionState()
	{
		if (m_isDragging)
			emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);

		m_isDragging = false;
		m_hoveredSubHandle = RotateHandle::SubHandle::none;
		m_activeSubHandle = RotateHandle::SubHandle::none;
		m_dragStartRotation = Float3x3::identity;
		m_dragStartMousePos = Float2::zero;
		m_dragAxisDir = Float3::zero;
	}



	// Mesh generation:
	void RotateHandle::CreateMeshes()
    {
        m_arcMesh = emberCore::MeshGenerator::ArcCurvedUv(s_arcStart, s_arcEnd, 90.0f, s_arcCornerCount);
    }





	void RotateHandle::TryBeginDrag()
    {
		// Return if no drag:
		if (m_isDragging || m_hoveredSubHandle == RotateHandle::SubHandle::none)
			return;
		if (!emberCore::EventSystem::MouseDown(emberCommon::Input::MouseButton::Left))
			return;
		if (!emberCore::EventSystem::TryLockMouseButton(emberCommon::Input::MouseButton::Left))
			return;

		// Begin drag:
		m_activeSubHandle = m_hoveredSubHandle;
		m_dragStartRotation = m_pTransform->GetRotation3x3();
		m_dragStartMousePos = HandleContext::GetViewportMousePos();
		m_dragAxisDir = Float3(HandleRotationMatrix() * Float4(SubHandleDirection(m_activeSubHandle), 0.0f));
		if (m_dragAxisDir.IsEpsilonZero())
		{
			m_activeSubHandle = RotateHandle::SubHandle::none;
			emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);
			return;
		}
		m_dragAxisDir = m_dragAxisDir.Normalize();

		// Start drag:
		m_isDragging = true;
		emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
    }
	void RotateHandle::UpdateDrag()
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

		// Update entity rotation:
		Float2 mouseDelta = HandleContext::GetViewportMousePos() - m_dragStartMousePos;
		float angle = s_rotationSensitivity * (mouseDelta.x - mouseDelta.y);
		m_pTransform->SetRotationMatrix(Float3x3::Rotate(m_dragAxisDir, angle) * m_dragStartRotation);
		emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
    }
	void RotateHandle::UpdateOctant()
    {
		if (m_isDragging)
			return;

		Float3 cameraDirection = HandleContext::GetCamera()->GetTransform()->GetPosition() - m_pTransform->GetPosition();
		if (cameraDirection.IsEpsilonZero())
			return;

		Float4x4 worldToHandleRotation = HandleRotationMatrix().Inverse();
		Float3 localCameraDirection = Float3(worldToHandleRotation * Float4(cameraDirection, 0.0f));
		if (localCameraDirection.IsEpsilonZero())
			return;

		m_octantIndex = 0;
		if (localCameraDirection.x < 0.0f)
			m_octantIndex |= 1;
		if (localCameraDirection.y < 0.0f)
			m_octantIndex |= 2;
		if (localCameraDirection.z < 0.0f)
			m_octantIndex |= 4;
    }
	void RotateHandle::UpdateHoveredSubHandle()
    {
		if (m_isDragging)
		{
			m_hoveredSubHandle = m_activeSubHandle;
			return;
		}

		m_hoveredSubHandle = RotateHandle::SubHandle::none;
		if (!HandleContext::GetViewPortIsHovered())
			return;

		// Raycast setup:
		Float4x4 localToWorldMatrix = LocalToWorldMatrix();
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		float closestHitDistanceSq = math::maxValue;

		// Check each axis handle:
		TryPickAxisSubHandle(RotateHandle::SubHandle::axisX, localToWorldMatrix * OctantMatrix(RotateHandle::SubHandle::axisX, m_octantIndex) * s_rotX, ray, closestHitDistanceSq);
		TryPickAxisSubHandle(RotateHandle::SubHandle::axisY, localToWorldMatrix * OctantMatrix(RotateHandle::SubHandle::axisY, m_octantIndex) * s_rotY, ray, closestHitDistanceSq);
		TryPickAxisSubHandle(RotateHandle::SubHandle::axisZ, localToWorldMatrix * OctantMatrix(RotateHandle::SubHandle::axisZ, m_octantIndex) * s_rotZ, ray, closestHitDistanceSq);
    }
		
    // Helpers:
	float RotateHandle::Size() const
    {
		if (!HasTarget())
			return 1.0f;
		return SizeAtPosition(m_pTransform->GetPosition());
    }
	float RotateHandle::SizeAtPosition(const Float3& position) const
    {
		return m_handleScale * HandleContext::GetGlobalHandleScale() * HandleContext::ComputeScreenSpaceScale(position);
    }
	Float4x4 RotateHandle::HandleRotationMatrix() const
    {
		// Local space: align handle with the target's rotation.
		// World space: keep handle aligned with the world coordinate axis.
		if (m_coordinateSpace == CoordinateSpace::local && HasTarget())
			return m_pTransform->GetRotation4x4();
		return Float4x4::identity;
    }
	Float4x4 RotateHandle::LocalToWorldMatrix()
    {
		if (!HasTarget())
			return Float4x4::identity;
		return Float4x4::TRS(m_pTransform->GetPosition(), HandleRotationMatrix(), Size());
    }
	Float4 RotateHandle::SubHandleStateColor(RotateHandle::SubHandle subHandle)
    {
		if (m_activeSubHandle == subHandle)
			return s_activeColor;
		if (m_hoveredSubHandle == subHandle)
			return s_hoverColor;
		return SubHandleBaseColor(subHandle);
    }
	void RotateHandle::TryPickAxisSubHandle(RotateHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq)
    {
		// Construct interaction quad matching the arc's visible quadrant:
		Float3 origin = Float3(localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
		Float3 uCorner = Float3(localToWorldMatrix * Float4(s_arcEnd, 0.0f, 0.0f, 1.0f));
		Float3 vCorner = Float3(localToWorldMatrix * Float4(0.0f, s_arcEnd, 0.0f, 1.0f));
		Quad quad = Quad(origin, uCorner - origin, vCorner - origin);

		// Ray-quad hit:
		std::optional<Float3> hit = quad.IntersectRay(ray);
		if (hit.has_value())
		{
			// Radius restriction:
			float innerRadius = s_arcStart * Size();
			float outerRadius = s_arcEnd * Size();
			float radiusSq = Float3::DistanceSq(origin, hit.value());
			if (radiusSq < innerRadius * innerRadius || radiusSq > outerRadius * outerRadius)
				return;

			// Check if new hit is closer:
			float hitDistanceSq = Float3::DistanceSq(ray.origin, hit.value());
			if (hitDistanceSq < closestHitDistanceSq)
			{
				closestHitDistanceSq = hitDistanceSq;
				m_hoveredSubHandle = subHandle;
			}
		}
    }

    // Static helpers:
	Float3 RotateHandle::SubHandleDirection(RotateHandle::SubHandle subHandle)
    {
		switch (subHandle)
		{
			case RotateHandle::SubHandle::axisX: return Float3::right;
			case RotateHandle::SubHandle::axisY: return Float3::forward;
			case RotateHandle::SubHandle::axisZ: return Float3::up;
			default: return Float3::zero;
        }
    }
	Float4 RotateHandle::SubHandleBaseColor(RotateHandle::SubHandle subHandle)
    {
		switch (subHandle)
		{
			case RotateHandle::SubHandle::axisX: return s_colorX;
			case RotateHandle::SubHandle::axisY: return s_colorY;
			case RotateHandle::SubHandle::axisZ: return s_colorZ;
			default: return Float4::zero;
		}
	}
	Float3 RotateHandle::OctantSigns(uint32_t octantIndex)
    {
		return Float3(
			(octantIndex & 1) == 0 ? 1.0f : -1.0f,
			(octantIndex & 2) == 0 ? 1.0f : -1.0f,
			(octantIndex & 4) == 0 ? 1.0f : -1.0f);
    }
	Float4x4 RotateHandle::OctantMatrix(RotateHandle::SubHandle subHandle, uint32_t octantIndex)
	{
		Float3 signs = OctantSigns(octantIndex);
		float sign0 = 1.0f;
		float sign1 = 1.0f;
		switch (subHandle)
		{
			case RotateHandle::SubHandle::axisX:
				sign0 = signs.y;
				sign1 = signs.z;
				break;
			case RotateHandle::SubHandle::axisY:
				sign0 = signs.z;
				sign1 = signs.x;
				break;
			case RotateHandle::SubHandle::axisZ:
				sign0 = signs.x;
				sign1 = signs.y;
				break;
			default:
				return Float4x4::identity;
		}

		Float3 axis = SubHandleDirection(subHandle);
		if (sign0 < 0.0f && sign1 > 0.0f)
			return Float4x4::Rotate(axis, math::pi2);
		if (sign0 < 0.0f && sign1 < 0.0f)
			return Float4x4::Rotate(axis, math::pi);
		if (sign0 > 0.0f && sign1 < 0.0f)
			return Float4x4::Rotate(axis, -math::pi2);
		return Float4x4::identity;
	}
}