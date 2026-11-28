#include "editorSelection.h"
#include "camera.h"
#include "emberMath.h"
#include "entity.inl"
#include "handleContext.h"
#include "meshRenderer.h"
#include "transform.h"
#include "world.h"



namespace emberEditor
{
	// Static members:
	emberEcs::EntityHandle EditorSelection::s_selectedEntity;



	// Public Methods:
	void EditorSelection::SetSelectedEntity(emberEcs::EntityHandle entity)
	{
		s_selectedEntity = entity;
	}
	emberEcs::EntityHandle EditorSelection::GetSelectedEntity()
	{
		return s_selectedEntity;
	}
	bool EditorSelection::HasSelectedEntity()
	{
		return s_selectedEntity.IsValid();
	}
	void EditorSelection::ClearSelection()
	{
		s_selectedEntity = emberEcs::EntityHandle();
	}
	bool EditorSelection::SelectEntityByMouse()
	{
		emberEcs::Camera* pCamera = HandleContext::GetCamera();
		if (pCamera == nullptr)
		{
			ClearSelection();
			return false;
		}

		Ray worldRay = pCamera->GetViewportRay(HandleContext::GetViewportMousePos01());
		float closestDistanceSq = math::maxValue;
		emberEcs::EntityHandle closestEntity;

		for (emberEcs::EntityHandle handle : emberEcs::World::GetEntities())
		{
			emberEcs::Entity& entity = *handle.Get();
			if (!entity.GetIsActive())
				continue;

			emberEcs::MeshRenderer* pMeshRenderer = entity.GetComponent<emberEcs::MeshRenderer>();
			if (pMeshRenderer == nullptr
				|| !pMeshRenderer->GetIsActive()
				|| !pMeshRenderer->GetRaycastEnabled()
				|| !pMeshRenderer->HasMesh())
				continue;

			Float4x4 worldToLocalMatrix = entity.GetTransform()->GetWorldToLocalMatrix();
			Ray localRay
			(
				Float3(worldToLocalMatrix * Float4(worldRay.origin, 1.0f)),
				Float3(worldToLocalMatrix * Float4(worldRay.direction, 0.0f))
			);
			RayMeshHit hit = pMeshRenderer->GetMesh().IntersectRay(localRay);
			if (!hit.GetHit())
				continue;

			Float4x4 localToWorldMatrix = entity.GetTransform()->GetLocalToWorldMatrix();
			Float3 worldHitPoint = Float3(localToWorldMatrix * Float4(hit.GetPoint(), 1.0f));
			float distanceSq = Float3::DistanceSq(worldRay.origin, worldHitPoint);
			if (distanceSq >= closestDistanceSq)
				continue;

			closestDistanceSq = distanceSq;
			closestEntity = handle;
		}

		s_selectedEntity = closestEntity;
		return closestEntity.IsValid();
	}
}