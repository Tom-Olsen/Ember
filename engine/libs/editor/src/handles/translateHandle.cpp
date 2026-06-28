#include "translateHandle.h"
#include "camera.h"
#include "eventSystem.h"
#include "gizmo.h"
#include "handleContext.h"
#include "materialManager.h"
#include "meshGenerator.h"
#include "transform.h"
#include <optional>



namespace emberEditor
{
	// Static members:
	Float4x4 TranslateHandle::s_rotX = Float4x4::RotateFromTo(Float3::up, Float3::right) * Float4x4::RotateZ(math::pi2);
	Float4x4 TranslateHandle::s_rotY = Float4x4::RotateFromTo(Float3::up, Float3::forward) * Float4x4::RotateZ(-math::pi2);
	Float4x4 TranslateHandle::s_rotZ = Float4x4::identity;
	float TranslateHandle::s_capsuleStart = 0.1f;
	float TranslateHandle::s_capsuleEnd = 1.0f;
	float TranslateHandle::s_capsuleWidth = 0.1f;



	// Public methods:
	// Constructor/Destructor:
	TranslateHandle::TranslateHandle()
	{
		HandleContext::Init();
		m_handleScale = 0.5f;//3.0f;
		m_material = emberCore::MaterialManager::GetMaterial("translateHandleMaterial");
		ResetInteractionState();
		CreateMeshes();
	}
	TranslateHandle::~TranslateHandle()
	{

	}



	// Target:
	void TranslateHandle::SetTarget(emberEcs::Transform* pTransform)
	{
		if (m_pTransform == pTransform)
			return;
		ResetInteractionState();
		m_pTransform = pTransform;
	}
	void TranslateHandle::ClearTarget()
	{
		m_pTransform = nullptr;
		ResetInteractionState();
	}
	bool TranslateHandle::HasTarget() const
	{
		return m_pTransform != nullptr;
	}



	// Handle:
	TranslateHandle::SubHandle TranslateHandle::GetHoveredSubHandle() const
	{
		return m_hoveredSubHandle;
	}
	TranslateHandle::SubHandle TranslateHandle::GetActiveSubHandle() const
	{
		return m_activeSubHandle;
	}
	bool TranslateHandle::IsDragging() const
	{
		return m_isDragging;
	}



	// Update/Draw:
	void TranslateHandle::Update()
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
	void TranslateHandle::Draw()
	{
		if (!HasTarget())
			return;
		Float4x4 localToWorldMatrix = LocalToWorldMatrix();

		emberCore::Gizmo::SetMaterial(m_material);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisX, Float4::red));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * s_rotX);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisY, Float4::green));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * s_rotY);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisZ, Float4::blue));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * s_rotZ);
		emberCore::Gizmo::ResetMaterial();

		// Visualize interaction regions
		//emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("transparentGizmoMaterial"));
		//emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisX, Float4::red) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * s_rotX);
		//emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisY, Float4::green) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * s_rotY);
		//emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisZ, Float4::blue) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * s_rotZ);
		//emberCore::Gizmo::ResetMaterial();
	}



	// Private methods:
	void TranslateHandle::ResetInteractionState()
	{
		if (m_isDragging)
			emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);

		m_pTransform = nullptr;
		m_isDragging = false;
		m_hoveredSubHandle = TranslateHandle::SubHandle::none;
		m_activeSubHandle = TranslateHandle::SubHandle::none;
		m_dragStartPosition = Float3::zero;
		m_dragStartHitPoint = Float3::zero;
		m_dragAxisDir = Float3::zero;
		m_dragPlaneNormal = Float3::zero;
	}



	// Mesh generation:
	void TranslateHandle::CreateMeshes()
	{
		// Arrow Mesh:
		{
			float bodyHeight = 0.8f;
			float bodyRadius = 0.02f;
			float headHeight = 0.2f;
			float headRadius = 0.1f;
			float cornerCount = 16;

			std::vector<emberCore::Mesh> meshes;
			meshes.reserve(4);
			meshes.emplace_back(std::move(emberCore::MeshGenerator::Disk(bodyRadius, cornerCount, "arrowFlat0").Rotate(Float3x3::rot180x)));
			meshes.emplace_back(std::move(emberCore::MeshGenerator::ZylinderMantleSmooth(bodyRadius, bodyHeight, cornerCount, "arrowFlat1").Translate(0.5f * bodyHeight * Float3::up)));
			meshes.emplace_back(std::move(emberCore::MeshGenerator::ArcFlatUv(bodyRadius, headRadius, 360.0f, cornerCount + 1, "arrowFlat2").Translate(-bodyHeight * Float3::up).Rotate(Float3x3::rot180x)));
			meshes.emplace_back(std::move(emberCore::MeshGenerator::ConeMantleSmooth(headRadius, headHeight, cornerCount, "arrowFlat3").Translate(bodyHeight * Float3::up)));
			m_arrowMesh = emberCore::Mesh::Merge(meshes, "translateHandleArrow");
		}

		// Capsule mesh:
		{
			float height = s_capsuleEnd - s_capsuleStart - 2.0f * s_capsuleWidth;
			Float3 translation = 0.5f * (s_capsuleEnd + s_capsuleStart) * Float3::up;
			m_capsuleMesh = emberCore::MeshGenerator::Capsule(s_capsuleWidth, height, 8, "translateHandleCapsule").Translate(translation);
		}
	}

	// Interaction:
	void TranslateHandle::TryBeginDrag()
	{
        // Return if no drag:
		if (m_isDragging || m_hoveredSubHandle == TranslateHandle::SubHandle::none)
			return;
		if (!emberCore::EventSystem::MouseDown(emberCommon::Input::MouseButton::Left))
			return;
		if (!emberCore::EventSystem::TryLockMouseButton(emberCommon::Input::MouseButton::Left))
			return;

        // Begin drag:
		m_activeSubHandle = m_hoveredSubHandle;
		m_dragStartPosition = m_pTransform->GetPosition();
		m_dragAxisDir = SubHandleAxisDirection(m_activeSubHandle);

        // Find drag plane normal:
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		m_dragPlaneNormal = ray.direction - Float3::Dot(ray.direction, m_dragAxisDir) * m_dragAxisDir;
        if (m_dragPlaneNormal.IsEpsilonZero())
		{
			m_activeSubHandle = TranslateHandle::SubHandle::none;
			emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);
			return;
		}
		m_dragPlaneNormal = m_dragPlaneNormal.Normalize();

        // Find initial hit point on drag plane:
		std::optional<Float3> hit = ray.HitOnPlane(m_dragStartPosition, m_dragPlaneNormal);
		if (!hit.has_value())
		{
			m_activeSubHandle = TranslateHandle::SubHandle::none;
			emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);
			return;
		}
		m_dragStartHitPoint = hit.value();

        // Start drag:
		m_isDragging = true;
		emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
	}
	void TranslateHandle::UpdateDrag()
	{
		if (!m_isDragging)
			return;

        // Cancel drag:
		if (emberCore::EventSystem::MouseUp(emberCommon::Input::MouseButton::Left))
		{
			emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
		    emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);
		    m_isDragging = false;
		    m_activeSubHandle = TranslateHandle::SubHandle::none;
		    m_dragStartPosition = Float3::zero;
		    m_dragStartHitPoint = Float3::zero;
		    m_dragAxisDir = Float3::zero;
		    m_dragPlaneNormal = Float3::zero;
			return;
		}

        // Update entity (and handle) position:
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		std::optional<Float3> hit = ray.HitOnPlane(m_dragStartPosition, m_dragPlaneNormal);
		if (hit.has_value())
		{
			float axisDistance = Float3::Dot(hit.value() - m_dragStartHitPoint, m_dragAxisDir);
			m_pTransform->SetPosition(m_dragStartPosition + axisDistance * m_dragAxisDir);
		}
		emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
	}
	void TranslateHandle::UpdateHoveredSubHandle()
	{
		if (m_isDragging)
		{
			m_hoveredSubHandle = m_activeSubHandle;
			return;
		}

		m_hoveredSubHandle = TranslateHandle::SubHandle::none;
		if (!HandleContext::GetViewPortIsHovered())
			return;

		// Raycast setup:
		Float4x4 localToWorldMatrix = LocalToWorldMatrix();
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		float closestHitDistanceSq = math::maxValue;

		// Check each handle:
		TryPickSubHandle(TranslateHandle::SubHandle::axisX, localToWorldMatrix * s_rotX, ray, closestHitDistanceSq);
		TryPickSubHandle(TranslateHandle::SubHandle::axisY, localToWorldMatrix * s_rotY, ray, closestHitDistanceSq);
		TryPickSubHandle(TranslateHandle::SubHandle::axisZ, localToWorldMatrix * s_rotZ, ray, closestHitDistanceSq);
	}



	// Helpers:
	float TranslateHandle::Size() const
	{
		if (!HasTarget())
			return 1.0f;
		return m_handleScale * HandleContext::GetGlobalHandleScale() * HandleContext::ComputeScreenSpaceScale(m_pTransform->GetPosition());
	}
	Float4x4 TranslateHandle::LocalToWorldMatrix()
	{
		if (!HasTarget())
			return Float4x4::identity;
		return Float4x4::TS(m_pTransform->GetPosition(), Size());
	}
	Float4 TranslateHandle::SubHandleColor(TranslateHandle::SubHandle subHandle, const Float4& baseColor)
	{
		if (m_activeSubHandle == subHandle)
			return Float4::white;
		if (m_hoveredSubHandle == subHandle)
			return Float4::yellow;
		return baseColor;
	}
	void TranslateHandle::TryPickSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& axisLocalToWorldMatrix, const Ray& ray, float& closestHitDistanceSq)
	{
		// Construct interaction capsule:
		Float3 point0 = Float3(axisLocalToWorldMatrix * Float4(0.0f, 0.0f, s_capsuleStart + s_capsuleWidth, 1.0f));
		Float3 point1 = Float3(axisLocalToWorldMatrix * Float4(0.0f, 0.0f, s_capsuleEnd - s_capsuleWidth, 1.0f));
		Capsule capsule = Capsule(point0, point1, s_capsuleWidth * Size());

		// Ray-capsule hit:
		std::optional<Float3> hit = capsule.IntersectRay(ray);
		if (!hit.has_value())
			return;

		// Check if new hit is closer:
		float hitDistance = Float3::DistanceSq(ray.origin, hit.value());
		if (hitDistance < closestHitDistanceSq)
		{
			closestHitDistanceSq = hitDistance;
			m_hoveredSubHandle = subHandle;
		}
	};



    // Static helpers:
	Float3 TranslateHandle::SubHandleAxisDirection(TranslateHandle::SubHandle subHandle)
	{
		switch (subHandle)
		{
			case TranslateHandle::SubHandle::axisX: return Float3::right;
			case TranslateHandle::SubHandle::axisY: return Float3::forward;
			case TranslateHandle::SubHandle::axisZ: return Float3::up;
			default: return Float3::zero;
        }
	}
}