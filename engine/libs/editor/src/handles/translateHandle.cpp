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
    // Handle colors:
	Float4 TranslateHandle::s_colorX = Float4(1.0f, 0.38039, 0.24314, 1.0f);
	Float4 TranslateHandle::s_colorY = Float4(0.78039f, 1.0f, 0.41569, 1.0f);
	Float4 TranslateHandle::s_colorZ = Float4(0.31765f, 0.66667f, 1.0f, 1.0f);
	Float4 TranslateHandle::s_hoverColor = Float4::yellow;
	Float4 TranslateHandle::s_activeColor = Float4::white;
    // Rotation matrizes:
	Float4x4 TranslateHandle::s_rotX = Float4x4::RotateFromTo(Float3::up, Float3::right) * Float4x4::RotateZ(math::pi2);
	Float4x4 TranslateHandle::s_rotY = Float4x4::RotateFromTo(Float3::up, Float3::forward) * Float4x4::RotateZ(-math::pi2);
	Float4x4 TranslateHandle::s_rotZ = Float4x4::identity;
    // Interaction capsule geometry:
	float TranslateHandle::s_capsuleStart = 0.25f;
	float TranslateHandle::s_capsuleEnd = 1.0f;
	float TranslateHandle::s_capsuleWidth = 0.1f;
    // Arrow handle geometry:
	float TranslateHandle::s_arrowBodyHeight = 0.8f;
	float TranslateHandle::s_arrowBodyRadius = 0.02f;
	float TranslateHandle::s_arrowHeadHeight = 0.2f;
	float TranslateHandle::s_arrowHeadRadius = 5.0f * s_arrowBodyRadius;
	float TranslateHandle::s_arrowCornerCount = 16;



	// Public methods:
	// Constructor/Destructor:
	TranslateHandle::TranslateHandle()
	{
		HandleContext::Init();
		m_handleScale = 3.0f;
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

        // Draw Arrows:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitMaterial"));
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisX, s_colorX));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * s_rotX);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisY, s_colorY));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * s_rotY);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisZ, s_colorZ));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * s_rotZ);
        // Draw plane quads:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitTransparentMaterial"));
        emberCore::Gizmo::SetCullMode(emberCommon::CullMode::none);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::planeYZ, s_colorX) - 0.33f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_quadMesh, localToWorldMatrix * s_rotX);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::planeXZ, s_colorY) - 0.33f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_quadMesh, localToWorldMatrix * s_rotY);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::planeXY, s_colorZ) - 0.33f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_quadMesh, localToWorldMatrix * s_rotZ);
		emberCore::Gizmo::ResetCullMode();
        // Draw plane quad outlines:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitMaterial"));
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::planeYZ, s_colorX));
        emberCore::Gizmo::DrawMesh(m_quadOutlineMesh, localToWorldMatrix * s_rotY);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::planeXZ, s_colorY));
        emberCore::Gizmo::DrawMesh(m_quadOutlineMesh, localToWorldMatrix * s_rotZ);
		emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::planeXY, s_colorZ));
        emberCore::Gizmo::DrawMesh(m_quadOutlineMesh, localToWorldMatrix * s_rotX);
		emberCore::Gizmo::ResetMaterial();

		// Visualize interaction regions
		//emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitTransparentMaterial"));
		//emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisX, s_colorX) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * s_rotX);
		//emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisY, s_colorY) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * s_rotY);
		//emberCore::Gizmo::SetColor(SubHandleColor(TranslateHandle::SubHandle::axisZ, s_colorZ) - 0.5f * Float4::in);
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
			std::vector<emberCore::Mesh> meshes;
			meshes.reserve(4);
			meshes.emplace_back(std::move(emberCore::MeshGenerator::Disk(s_arrowBodyRadius, s_arrowCornerCount, "arrowFlat0").Rotate(Float3x3::rot180x)));
			meshes.emplace_back(std::move(emberCore::MeshGenerator::ZylinderMantleSmooth(s_arrowBodyRadius, s_arrowBodyHeight, s_arrowCornerCount, "arrowFlat1").Translate(0.5f * s_arrowBodyHeight * Float3::up)));
			meshes.emplace_back(std::move(emberCore::MeshGenerator::ArcFlatUv(s_arrowBodyRadius, s_arrowHeadRadius, 360.0f, s_arrowCornerCount + 1, "arrowFlat2").Translate(-s_arrowBodyHeight * Float3::up).Rotate(Float3x3::rot180x)));
			meshes.emplace_back(std::move(emberCore::MeshGenerator::ConeMantleSmooth(s_arrowHeadRadius, s_arrowHeadHeight, s_arrowCornerCount, "arrowFlat3").Translate(s_arrowBodyHeight * Float3::up)));
			m_arrowMesh = emberCore::Mesh::Merge(meshes);
		}

		// Capsule mesh:
		{
			float height = s_capsuleEnd - s_capsuleStart - 2.0f * s_capsuleWidth;
			Float3 translation = 0.5f * (s_capsuleEnd + s_capsuleStart) * Float3::up;
			m_capsuleMesh = emberCore::MeshGenerator::Capsule(s_capsuleWidth, height, 8, "translateHandleCapsule").Translate(translation);
		}

        // Quad mesh:
        {
            float scale = s_capsuleStart;
            Float4x4 transformMatrix = Float4x4::TS(scale * Float3(0.5f, 0.5f, 0.0f), scale);
            m_quadMesh = emberCore::MeshGenerator::Quad().Transform(transformMatrix);
        }

        // Quad outline mesh:
        {
            float cubeLength = s_capsuleStart + 0.5f * s_arrowBodyRadius;
            float cubeWidth = math::sqrt2 * s_arrowBodyRadius;
            float cubePos = s_capsuleStart;

            Float3 scale0 = Float3(cubeLength, cubeWidth, cubeWidth);
            Float4x4 transformMatrix0 = Float4x4::TS(Float3(0.5f * cubeLength, 0.0f, cubePos), scale0);
            Float3 scale1 = Float3(cubeWidth, cubeWidth, cubeLength);
            Float4x4 transformMatrix1 = Float4x4::TS(Float3(cubePos, 0.0f, 0.5f * cubeLength), scale1);

            std::vector<emberCore::Mesh> cubes;
			cubes.reserve(2);
            cubes.emplace_back(emberCore::MeshGenerator::Cube().Transform(transformMatrix0));
            cubes.emplace_back(emberCore::MeshGenerator::Cube().Transform(transformMatrix1));
            m_quadOutlineMesh = emberCore::Mesh::Merge(cubes);
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

        // Find drag plane/line:
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		Float3 worldDirection = Float3(m_pTransform->GetRotation4x4() * Float4(SubHandleDirection(m_activeSubHandle), 0.0f));
		if (IsPlaneSubHandle(m_activeSubHandle))
		{
			// Plane handle: move within the quad's plane.
			m_dragAxisDir = Float3::zero;
			m_dragPlaneNormal = worldDirection;
		}
		else
		{
			// Axis handle: move along one axis.
			m_dragAxisDir = worldDirection;
			m_dragPlaneNormal = ray.direction - Float3::Dot(ray.direction, m_dragAxisDir) * m_dragAxisDir;
			if (m_dragPlaneNormal.IsEpsilonZero())
			{
				m_activeSubHandle = TranslateHandle::SubHandle::none;
				emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);
				return;
			}
			m_dragPlaneNormal = m_dragPlaneNormal.Normalize();
		}

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
			if (IsPlaneSubHandle(m_activeSubHandle))
			{
				// Plane handle: move freely within the drag plane.
				m_pTransform->SetPosition(m_dragStartPosition + (hit.value() - m_dragStartHitPoint));
			}
			else
			{
				// Axis handle: constrain movement to the axis.
				float axisDistance = Float3::Dot(hit.value() - m_dragStartHitPoint, m_dragAxisDir);
				m_pTransform->SetPosition(m_dragStartPosition + axisDistance * m_dragAxisDir);
			}
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

		// Check each axis handle:
		TryPickAxisSubHandle(TranslateHandle::SubHandle::axisX, localToWorldMatrix * s_rotX, ray, closestHitDistanceSq);
		TryPickAxisSubHandle(TranslateHandle::SubHandle::axisY, localToWorldMatrix * s_rotY, ray, closestHitDistanceSq);
		TryPickAxisSubHandle(TranslateHandle::SubHandle::axisZ, localToWorldMatrix * s_rotZ, ray, closestHitDistanceSq);

		// Check each plane handle:
		TryPickPlaneSubHandle(TranslateHandle::SubHandle::planeXY, localToWorldMatrix, ray, closestHitDistanceSq);
		TryPickPlaneSubHandle(TranslateHandle::SubHandle::planeYZ, localToWorldMatrix, ray, closestHitDistanceSq);
		TryPickPlaneSubHandle(TranslateHandle::SubHandle::planeXZ, localToWorldMatrix, ray, closestHitDistanceSq);
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
		return Float4x4::TRS(m_pTransform->GetPosition(), m_pTransform->GetRotation4x4(), Size());
	}
	Float4 TranslateHandle::SubHandleColor(TranslateHandle::SubHandle subHandle, const Float4& baseColor)
	{
		if (m_activeSubHandle == subHandle)
			return s_activeColor;
		if (m_hoveredSubHandle == subHandle)
			return s_hoverColor;
		return baseColor;
	}
	void TranslateHandle::TryPickAxisSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& axisLocalToWorldMatrix, const Ray& ray, float& closestHitDistanceSq)
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
	void TranslateHandle::TryPickPlaneSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq)
	{
		// Construct interaction quad (15% bigger then visual size):
		float size = 1.15f * (s_capsuleStart + 0.5f * s_arrowBodyRadius);   
		Float4x4 fillMatrix = localToWorldMatrix;
		switch (subHandle)
		{
			case TranslateHandle::SubHandle::planeXY: fillMatrix *= s_rotZ; break;
			case TranslateHandle::SubHandle::planeYZ: fillMatrix *= s_rotX; break;
			case TranslateHandle::SubHandle::planeXZ: fillMatrix *= s_rotY; break;
		}
		Float3 origin = Float3(fillMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
		Float3 uCorner = Float3(fillMatrix * Float4(size, 0.0f, 0.0f, 1.0f));
		Float3 vCorner = Float3(fillMatrix * Float4(0.0f, size, 0.0f, 1.0f));
		Quad quad = Quad(origin, uCorner - origin, vCorner - origin);

		// Ray-quad hit:
		std::optional<Float3> hit = quad.IntersectRay(ray);
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
	Float3 TranslateHandle::SubHandleDirection(TranslateHandle::SubHandle subHandle)
	{
		switch (subHandle)
		{
			case TranslateHandle::SubHandle::axisX: return Float3::right;
			case TranslateHandle::SubHandle::axisY: return Float3::forward;
			case TranslateHandle::SubHandle::axisZ: return Float3::up;
			case TranslateHandle::SubHandle::planeXY: return Float3::up;
			case TranslateHandle::SubHandle::planeYZ: return Float3::right;
			case TranslateHandle::SubHandle::planeXZ: return Float3::forward;
			default: return Float3::zero;
        }
	}
	bool TranslateHandle::IsAxisSubHandle(TranslateHandle::SubHandle subHandle)
    {
		return subHandle == TranslateHandle::SubHandle::axisX
			|| subHandle == TranslateHandle::SubHandle::axisY
			|| subHandle == TranslateHandle::SubHandle::axisZ;
    }
	bool TranslateHandle::IsPlaneSubHandle(TranslateHandle::SubHandle subHandle)
	{
		return subHandle == TranslateHandle::SubHandle::planeXY
			|| subHandle == TranslateHandle::SubHandle::planeYZ
			|| subHandle == TranslateHandle::SubHandle::planeXZ;
	}
}