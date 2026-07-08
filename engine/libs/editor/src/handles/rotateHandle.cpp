#include "rotateHandle.h"
#include "eventSystem.h"
#include "gizmo.h"
#include "handleContext.h"
#include "material.h"
#include "materialManager.h"
#include "meshGenerator.h"
#include "shaderProperties.h"
#include "transform.h"



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
	float RotateHandle::s_arcCornerCount = 16;



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
        emberCore::Gizmo::DrawMesh(m_arcMesh, localToWorldMatrix * s_rotX);
		emberCore::Gizmo::SetColor(SubHandleStateColor(RotateHandle::SubHandle::axisY));
        emberCore::Gizmo::DrawMesh(m_arcMesh, localToWorldMatrix * s_rotY);
		emberCore::Gizmo::SetColor(SubHandleStateColor(RotateHandle::SubHandle::axisZ));
        emberCore::Gizmo::DrawMesh(m_arcMesh, localToWorldMatrix * s_rotZ);
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
	}



	// Mesh generation:
	void RotateHandle::CreateMeshes()
    {
        m_arcMesh = emberCore::MeshGenerator::ArcCurvedUv(s_arcStart, s_arcEnd, 90.0f, s_arcCornerCount);
    }





	void RotateHandle::TryBeginDrag()
    {

    }
	void RotateHandle::UpdateDrag()
    {

    }
	void RotateHandle::UpdateOctant()
    {

    }
	void RotateHandle::UpdateHoveredSubHandle()
    {

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
}