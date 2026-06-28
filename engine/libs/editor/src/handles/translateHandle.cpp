#include "translateHandle.h"
#include "camera.h"
#include "gizmo.h"
#include "handleContext.h"
#include "materialManager.h"
#include "meshGenerator.h"
#include "transform.h"



namespace emberEditor
{
	// Public methods:
    // Constructor/Destructor:
    TranslateHandle::TranslateHandle()
    {
        HandleContext::Init();
        m_handleScale = 3.0f;
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
		m_hoveredSubHandle = TranslateHandle::SubHandle::none;
	}
	void TranslateHandle::Draw()
	{
		if (!HasTarget())
			return;

		Float3 position = m_pTransform->GetPosition();
		float size = m_handleScale * HandleContext::GetGlobalHandleScale() * HandleContext::ComputeScreenSpaceScale(position);
        Float3x3 rotation = m_pTransform->GetRotation3x3();
        Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotation, size);
        static Float4x4 rotX = Float4x4::Rotate(Float3(0, 0, -math::pi2));
        static Float4x4 rotY = Float4x4::Rotate(Float3(math::pi2, 0, 0));
        static Float4x4 rotZ = Float4x4::Rotate(Float3(0, math::pi2, math::pi));

        emberCore::Gizmo::SetMaterial(m_material);
		emberCore::Gizmo::SetColor(Float4::red);
        emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * rotX);
		emberCore::Gizmo::SetColor(Float4::green);
        emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * rotY);
		emberCore::Gizmo::SetColor(Float4::blue);
        emberCore::Gizmo::DrawMesh(m_arrowMesh, localToWorldMatrix * rotZ);
        emberCore::Gizmo::ResetMaterial();

        // Visualize interaction capsules for testing.
        emberCore::Gizmo::SetMaterial(emberCore::MaterialManager::GetMaterial("transparentGizmoMaterial"));
		emberCore::Gizmo::SetColor(Float4::red - 0.5f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * rotX);
		emberCore::Gizmo::SetColor(Float4::green - 0.5f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * rotY);
		emberCore::Gizmo::SetColor(Float4::blue - 0.5f * Float4::in);
        emberCore::Gizmo::DrawMesh(m_capsuleMesh, localToWorldMatrix * rotZ);
        emberCore::Gizmo::ResetMaterial();
	}



	// Private methods:
	void TranslateHandle::ResetInteractionState()
	{
		m_pTransform = nullptr;
		m_isDragging = false;
		m_hoveredSubHandle = TranslateHandle::SubHandle::none;
		m_activeSubHandle = TranslateHandle::SubHandle::none;
        m_dragStartPosition = Float3::zero;
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
            float totalHeight = 0.9f;                                       // 80% of arrow length, to prevent overlap at origin.
            float interactionRadius = 0.1f;                                 // same as arrow head.
            float height = totalHeight - 2 * interactionRadius;             // 90% of arrow height, to prevent overlap at origin.
            Float3 translation = (0.5f * totalHeight + 0.1f) * Float3::up;  // +0.1 = 10% deadzone at origin.
            m_capsuleMesh = emberCore::MeshGenerator::Capsule(interactionRadius, height, 8, "translateHandleCapsule").Translate(translation);
        }
    }
}