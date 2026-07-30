#include "editorSelection.h"
#include "camera.h"
#include "emberMath.h"
#include "entity.inl"
#include "handleContext.h"
#include "meshRenderer.h"
#include "scene.h"
#include "transform.h"
#include <assert.h>



namespace emberEditor
{
	// Static members:
	std::optional<emberEcs::Entity> EditorSelection::s_selectedEntity = std::nullopt;



	// Public methods:
	void EditorSelection::SetSelectedEntity(const emberEcs::Entity& entity)
	{
		s_selectedEntity = entity;
	}
	emberEcs::Entity EditorSelection::GetSelectedEntity()
	{
		assert(s_selectedEntity.has_value());
		return s_selectedEntity.value();
	}
	bool EditorSelection::HasSelectedEntity()
	{
		return s_selectedEntity.has_value() && s_selectedEntity->GetIsValid();
	}
	void EditorSelection::ClearSelection()
	{
		s_selectedEntity = std::nullopt;
	}
	bool EditorSelection::SelectEntityByMouse(emberEcs::Scene& scene)
	{
		emberEcs::Camera* pCamera = HandleContext::GetCamera();
		if (pCamera == nullptr)
		{
			ClearSelection();
			return false;
		}

		Ray worldRay = pCamera->GetViewportRay(HandleContext::GetViewportMousePos01());
		float closestDistanceSq = math::maxValue;
		std::optional<emberEcs::Entity> closestEntity = std::nullopt;

		for (const std::string& entityName : scene.GetEntityNames())
		{
			emberEcs::Entity entity = scene.GetEntity(entityName);
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
			closestEntity = entity;
		}

		s_selectedEntity = closestEntity;
		return closestEntity.has_value();
	}
}