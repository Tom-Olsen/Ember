#include "translateHandle.h"
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
	float TranslateHandle::s_arrowHeight = 1.0f;
    float TranslateHandle::s_quadSize = 0.25f;
	float TranslateHandle::s_arrowBodyHeight = 0.8f;
	float TranslateHandle::s_arrowBodyRadius = 0.02f;
	float TranslateHandle::s_arrowHeadHeight = s_arrowHeight - s_arrowBodyHeight;
	float TranslateHandle::s_arrowHeadRadius = 5.0f * s_arrowBodyRadius;
	float TranslateHandle::s_arrowCornerCount = 16;



	// Public methods:
	// Constructor/Destructor:
	TranslateHandle::TranslateHandle()
	{
		m_handleScale = 1.0f;
		m_coordinateSpace = CoordinateSpace::world;
		m_pHandleTarget = nullptr;
		m_isDragging = false;
		m_octantIndex = 0;
		ResetInteractionState();
		CreateMeshes();
	}
	TranslateHandle::~TranslateHandle()
	{

	}



	// Target:
	void TranslateHandle::SetTarget(IHandleTarget* pHandleTarget)
	{
		if (m_pHandleTarget == pHandleTarget)
			return;
		ResetInteractionState();
		m_pHandleTarget = pHandleTarget;
	}
	void TranslateHandle::ClearTarget()
	{
		m_pHandleTarget = nullptr;
		ResetInteractionState();
	}
	bool TranslateHandle::HasTarget() const
	{
		return m_pHandleTarget != nullptr && m_pHandleTarget->CanTranslate();
	}



	// Coordinate space:
	void TranslateHandle::SetCoordinateSpace(CoordinateSpace coordinateSpace)
	{
		m_coordinateSpace = coordinateSpace;
	}
	CoordinateSpace TranslateHandle::GetCoordinateSpace() const
	{
		return m_coordinateSpace;
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
	bool TranslateHandle::GetIsDragging() const
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
		UpdateOctant();
		UpdateHoveredSubHandle();
		TryBeginDrag();
		UpdateDrag();
	}
	void TranslateHandle::Draw()
	{
		if (!HasTarget())
			return;
		Float4x4 localToWorldMatrix = LocalToWorldMatrix();

        // Draw central sphere:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoUnlitMaterial"));
		emberCore::Gizmo::SetColor(TransformHandle::GetColorCenter());
        emberCore::Gizmo::DrawSphere(localToWorldMatrix * Float4x4::Scale(0.15f));
        // Draw Arrows:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitMaterial"));
		emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::axisX));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * TransformHandle::GetRotationX());
		emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::axisY));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * TransformHandle::GetRotationY());
		emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::axisZ));
		emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * TransformHandle::GetRotationZ());
        // Draw plane quads:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitTransparentMaterial"));
        emberCore::Gizmo::SetCullMode(emberCommon::CullMode::none);
		float quadSize = s_quadSize + 0.5f * math::sqrt2 * s_arrowBodyRadius;
		emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::planeYZ) - 0.33f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_quadMesh, localToWorldMatrix * TransformHandle::GetRotationX() * PlaneQuadTranslation(TranslateHandle::SubHandle::planeYZ, m_octantIndex, quadSize));
		emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::planeXZ) - 0.33f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_quadMesh, localToWorldMatrix * TransformHandle::GetRotationY() * PlaneQuadTranslation(TranslateHandle::SubHandle::planeXZ, m_octantIndex, quadSize));
		emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::planeXY) - 0.33f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_quadMesh, localToWorldMatrix * TransformHandle::GetRotationZ() * PlaneQuadTranslation(TranslateHandle::SubHandle::planeXY, m_octantIndex, quadSize));
		emberCore::Gizmo::ResetCullMode();

        // Draw plane frame:
		emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoVertexColorLitMaterial"));
		emberCore::ShaderProperties shaderProperties = emberCore::Gizmo::DrawMesh(m_frameMeshes[m_octantIndex], localToWorldMatrix);
		int state = IsPlaneSubHandle(m_activeSubHandle) ? 2 : IsPlaneSubHandle(m_hoveredSubHandle) ? 1 : 0;
		Float4 stateColor = (state == 2) ? SubHandleBaseColor(m_activeSubHandle) : (state == 1) ? SubHandleBaseColor(m_hoveredSubHandle) : Float4::zero;
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		shaderProperties.SetValue("SelectionState", "state", state);
		shaderProperties.SetValue("SelectionState", "stateColor", stateColor);
		shaderProperties.SetValue("SelectionState", "hoverColor", TransformHandle::GetHoverColor());
		shaderProperties.SetValue("SelectionState", "activeColor", TransformHandle::GetActiveColor());
		emberCore::Gizmo::ResetMaterial();

		// Visualize arrow interaction regions
		//emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("gizmoLitMaterial"));
		//emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::axisX) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * TransformHandle::GetRotationX());
		//emberCore::Gizmo::DrawMesh(m_arrowHeadCapsuleMesh, localToWorldMatrix * TransformHandle::GetRotationX());
		//emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::axisY) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * TransformHandle::GetRotationY());
		//emberCore::Gizmo::DrawMesh(m_arrowHeadCapsuleMesh, localToWorldMatrix * TransformHandle::GetRotationY());
		//emberCore::Gizmo::SetColor(SubHandleStateColor(TranslateHandle::SubHandle::axisZ) - 0.5f * Float4::in);
		//emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * TransformHandle::GetRotationZ());
		//emberCore::Gizmo::DrawMesh(m_arrowHeadCapsuleMesh, localToWorldMatrix * TransformHandle::GetRotationZ());
		//emberCore::Gizmo::ResetMaterial();
	}



	// Private methods:
	void TranslateHandle::ResetInteractionState()
	{
		if (m_isDragging)
			emberCore::EventSystem::UnlockMouseButton(emberCommon::Input::MouseButton::Left);

		m_isDragging = false;
		m_hoveredSubHandle = TranslateHandle::SubHandle::none;
		m_activeSubHandle = TranslateHandle::SubHandle::none;
		m_dragStartPosition = Float3::zero;
		m_dragStartHitPoint = Float3::zero;
		m_dragAxisDir = Float3::zero;
		m_dragPlaneNormal = Float3::zero;
		m_dragGrabOffset = Float3::zero;
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
            float radius = 2.0f * s_arrowBodyRadius;
			float height = (s_arrowHeight - s_quadSize) - 2.0f * radius;
			Float3 translation = (s_arrowHeight - radius - 0.5f * height) * Float3::up;
			m_capsuleMesh = emberCore::MeshGenerator::Capsule(radius, height, 8, "translateHandleCapsule").Translate(translation);
		}

		// Arrow head capsule mesh:
		{
			float height = s_arrowHeadHeight - s_arrowHeadRadius;
			Float3 translation = (s_arrowBodyHeight + 0.5f * height) * Float3::up;
			m_arrowHeadCapsuleMesh = emberCore::MeshGenerator::Capsule(s_arrowHeadRadius, height, 8, "translateHandleArrowHeadCapsule").Translate(translation);
		}

        // Quad mesh:
        {
            float cubeWidth = math::sqrt2 * s_arrowBodyRadius;
            float size = s_quadSize + 0.5f * cubeWidth;
            Float4x4 transformMatrix = Float4x4::TS(size * Float3(0.5f, 0.5f, 0.0f), size);
            m_quadMesh = emberCore::MeshGenerator::Quad().Transform(transformMatrix);
        }

        // Frame mesh:
        {
            float frameWidth = math::sqrt2 * s_arrowBodyRadius;
            float frameLength = s_quadSize - frameWidth;
            emberCore::Mesh baseFrame = emberCore::MeshGenerator::TranslateHandleFrame(frameWidth, frameLength, TransformHandle::GetColorX(), TransformHandle::GetColorY(), TransformHandle::GetColorZ());
			for (uint32_t octantIndex = 0; octantIndex < 8; octantIndex++)
			{
				m_frameMeshes[octantIndex] = baseFrame.GetCopy();
		        Float3 signs = OctantSigns(octantIndex);
		        if (signs.x < 0.0f)
		        	m_frameMeshes[octantIndex].Mirror(Float3::zero, Float3::right);
		        if (signs.y < 0.0f)
		        	m_frameMeshes[octantIndex].Mirror(Float3::zero, Float3::forward);
		        if (signs.z < 0.0f)
		        	m_frameMeshes[octantIndex].Mirror(Float3::zero, Float3::up);
			}
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
		m_dragStartPosition = m_pHandleTarget->GetPosition();

        // Find drag plane/line:
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		Float3 worldDirection = Float3(HandleRotationMatrix() * Float4(SubHandleDirection(m_activeSubHandle), 0.0f));
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
		m_dragGrabOffset = (m_dragStartHitPoint - m_dragStartPosition) / Size();

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
		    ResetInteractionState();
			return;
		}

        // Update entity (and handle) position:
		Ray ray = HandleContext::GetCamera()->GetViewportRay(HandleContext::GetViewportMousePos01());
		std::optional<Float3> hit = ray.HitOnPlane(m_dragStartPosition, m_dragPlaneNormal);
		if (hit.has_value())
		{
			if (IsPlaneSubHandle(m_activeSubHandle))
			{
				// Small fixed point iteration:
				Float3 position = m_pHandleTarget->GetPosition();
				for (int i = 0; i < 4; i++)
					position = hit.value() - SizeAtPosition(position) * m_dragGrabOffset;
				m_pHandleTarget->SetPosition(position);
			}
			else // Axis translation:
			{
				// ResolveAxisDragPosition:
				Float3 position = m_pHandleTarget->GetPosition();
				float grabOffset = Float3::Dot(m_dragGrabOffset, m_dragAxisDir);
				float hitDistance = Float3::Dot(hit.value() - m_dragStartPosition, m_dragAxisDir);
				// Small fixed point iteration:
				for (int i = 0; i < 4; i++)
				{
					float axisDistance = hitDistance - SizeAtPosition(position) * grabOffset;
					position = m_dragStartPosition + axisDistance * m_dragAxisDir;
				}
				m_pHandleTarget->SetPosition(position);
			}
		}
		emberCore::EventSystem::ConsumeMouseButton(emberCommon::Input::MouseButton::Left);
	}
	void TranslateHandle::UpdateOctant()
	{
		if (m_isDragging)
			return;

		Float3 cameraDirection = HandleContext::GetCamera()->GetTransform()->GetPosition() - m_pHandleTarget->GetPosition();
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
		TryPickAxisSubHandle(TranslateHandle::SubHandle::axisX, localToWorldMatrix * TransformHandle::GetRotationX(), ray, closestHitDistanceSq);
		TryPickAxisSubHandle(TranslateHandle::SubHandle::axisY, localToWorldMatrix * TransformHandle::GetRotationY(), ray, closestHitDistanceSq);
		TryPickAxisSubHandle(TranslateHandle::SubHandle::axisZ, localToWorldMatrix * TransformHandle::GetRotationZ(), ray, closestHitDistanceSq);

		// Check each plane handle:
		float cubeWidth = math::sqrt2 * s_arrowBodyRadius;
		float quadSize = s_quadSize + 0.5f * cubeWidth;
		TryPickPlaneSubHandle(TranslateHandle::SubHandle::planeXY, localToWorldMatrix * TransformHandle::GetRotationZ() * PlaneQuadTranslation(TranslateHandle::SubHandle::planeXY, m_octantIndex, quadSize), ray, closestHitDistanceSq);
		TryPickPlaneSubHandle(TranslateHandle::SubHandle::planeYZ, localToWorldMatrix * TransformHandle::GetRotationX() * PlaneQuadTranslation(TranslateHandle::SubHandle::planeYZ, m_octantIndex, quadSize), ray, closestHitDistanceSq);
		TryPickPlaneSubHandle(TranslateHandle::SubHandle::planeXZ, localToWorldMatrix * TransformHandle::GetRotationY() * PlaneQuadTranslation(TranslateHandle::SubHandle::planeXZ, m_octantIndex, quadSize), ray, closestHitDistanceSq);
	}



	// Helpers:
	float TranslateHandle::Size() const
	{
		if (!HasTarget())
			return 1.0f;
		return SizeAtPosition(m_pHandleTarget->GetPosition());
	}
	float TranslateHandle::SizeAtPosition(const Float3& position) const
	{
		return m_handleScale * HandleContext::GetGlobalHandleScale() * HandleContext::ComputeScreenSpaceScale(position);
	}
	Float4x4 TranslateHandle::HandleRotationMatrix() const
	{
		// Local space: align handle with the target's rotation (requires rotation capabilities).
		// World space: keep handle aligned with the world coordinate axis.
		if (m_coordinateSpace == CoordinateSpace::local && HasTarget() && m_pHandleTarget->CanRotate())
			return m_pHandleTarget->GetRotation4x4();
		return Float4x4::identity;
	}
	Float4x4 TranslateHandle::LocalToWorldMatrix()
	{
		if (!HasTarget())
			return Float4x4::identity;
		return Float4x4::TRS(m_pHandleTarget->GetPosition(), HandleRotationMatrix(), Size());
	}
	Float4 TranslateHandle::SubHandleStateColor(TranslateHandle::SubHandle subHandle)
	{
		if (m_activeSubHandle == subHandle)
			return TransformHandle::GetActiveColor();
		if (m_hoveredSubHandle == subHandle)
			return TransformHandle::GetHoverColor();
		return SubHandleBaseColor(subHandle);
	}
	void TranslateHandle::TryPickAxisSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq)
	{
		// Construct interaction capsule:
        float radius = 2.0f * s_arrowBodyRadius;
		Float3 point0 = Float3(localToWorldMatrix * Float4(0.0f, 0.0f, s_quadSize + radius, 1.0f));
		Float3 point1 = Float3(localToWorldMatrix * Float4(0.0f, 0.0f, s_arrowHeight - radius, 1.0f));
		Capsule capsule = Capsule(point0, point1, radius * Size());

		// Ray-capsule hit:
		std::optional<Float3> hit = capsule.IntersectRay(ray);
		if (hit.has_value())
		{
			float hitDistanceSq = Float3::DistanceSq(ray.origin, hit.value());
			if (hitDistanceSq < closestHitDistanceSq)
			{
				closestHitDistanceSq = hitDistanceSq;
				m_hoveredSubHandle = subHandle;
			}
		}

		// Construct arrow head interaction capsule:
		Float3 headPoint0 = Float3(localToWorldMatrix * Float4(0.0f, 0.0f, s_arrowBodyHeight, 1.0f));
		Float3 headPoint1 = Float3(localToWorldMatrix * Float4(0.0f, 0.0f, s_arrowHeight - s_arrowHeadRadius, 1.0f));
		Capsule headCapsule = Capsule(headPoint0, headPoint1, s_arrowHeadRadius * Size());

		// Ray-arrow-head-capsule hit:
		hit = headCapsule.IntersectRay(ray);
		if (hit.has_value())
		{
			float hitDistanceSq = Float3::DistanceSq(ray.origin, hit.value());
			if (hitDistanceSq < closestHitDistanceSq)
			{
				closestHitDistanceSq = hitDistanceSq;
				m_hoveredSubHandle = subHandle;
			}
		}
	};
	void TranslateHandle::TryPickPlaneSubHandle(TranslateHandle::SubHandle subHandle, const Float4x4& localToWorldMatrix, const Ray& ray, float& closestHitDistanceSq)
	{
		// Construct interaction quad (15% bigger then visual size):
        float cubeWidth = math::sqrt2 * s_arrowBodyRadius;
        float size = s_quadSize + 0.5f * cubeWidth;
		Float3 origin = Float3(localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
		Float3 uCorner = Float3(localToWorldMatrix * Float4(size, 0.0f, 0.0f, 1.0f));
		Float3 vCorner = Float3(localToWorldMatrix * Float4(0.0f, size, 0.0f, 1.0f));
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
	Float4 TranslateHandle::SubHandleBaseColor(TranslateHandle::SubHandle subHandle)
	{
		switch (subHandle)
		{
			case TranslateHandle::SubHandle::axisX: return TransformHandle::GetColorX();
			case TranslateHandle::SubHandle::axisY: return TransformHandle::GetColorY();
			case TranslateHandle::SubHandle::axisZ: return TransformHandle::GetColorZ();
			case TranslateHandle::SubHandle::planeYZ: return TransformHandle::GetColorX();
			case TranslateHandle::SubHandle::planeXZ: return TransformHandle::GetColorY();
			case TranslateHandle::SubHandle::planeXY: return TransformHandle::GetColorZ();
			default: return Float4::zero;
		}
	}
	Float3 TranslateHandle::OctantSigns(uint32_t octantIndex)
	{
		return Float3(
			(octantIndex & 1) == 0 ? 1.0f : -1.0f,
			(octantIndex & 2) == 0 ? 1.0f : -1.0f,
			(octantIndex & 4) == 0 ? 1.0f : -1.0f);
	}
	Float4x4 TranslateHandle::PlaneQuadTranslation(TranslateHandle::SubHandle subHandle, uint32_t octantIndex, float size)
	{
		Float3 signs = OctantSigns(octantIndex);
		Float3 translation = Float3::zero;
		switch (subHandle)
		{
			case TranslateHandle::SubHandle::planeXY:
				translation.x = signs.x < 0.0f ? -size : 0.0f;
				translation.y = signs.y < 0.0f ? -size : 0.0f;
				break;
			case TranslateHandle::SubHandle::planeYZ:
				translation.x = signs.y < 0.0f ? -size : 0.0f;
				translation.y = signs.z < 0.0f ? -size : 0.0f;
				break;
			case TranslateHandle::SubHandle::planeXZ:
				translation.x = signs.z < 0.0f ? -size : 0.0f;
				translation.y = signs.x < 0.0f ? -size : 0.0f;
				break;
		}
		return Float4x4::Translate(translation);
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