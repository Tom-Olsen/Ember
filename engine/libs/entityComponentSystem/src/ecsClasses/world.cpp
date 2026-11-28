#include "world.h"
#include "bounds.h"
#include "camera.h"
#include "componentRegistry.h"
#include "entity.h"
#include "entity.inl"
#include "meshRenderer.h"
#include "transform.h"



namespace emberEcs
{
	// Static members:
	std::unordered_map<EntityId, std::unique_ptr<Entity>> World::s_entities;
	entt::registry World::s_registry;
	EntityId World::s_nextEntityId = 1;
	uint64_t World::s_nextRuntimeId = 1;
	EntityHandle World::s_activeCameraEntity;
	emberMath::Bounds World::s_bounds;



	// Public Methods:
	// Setters:
	void World::SetActiveCamera(Camera* pCamera)
	{
		s_activeCameraEntity = pCamera != nullptr ? pCamera->GetEntity() : EntityHandle();
	}



	// Getters:
	Camera* World::GetActiveCamera()
	{
		Entity* pEntity = s_activeCameraEntity.Get();
		return pEntity != nullptr ? pEntity->GetComponent<Camera>() : nullptr;
	}
	emberMath::Bounds World::GetBounds()
	{
		return s_bounds;
	}



	// Entity management:
	EntityHandle World::CreateEntity(const std::string& name)
	{
		const EntityId id = s_nextEntityId++;
		const uint64_t runtimeId = s_nextRuntimeId++;
		const entt::entity registryEntity = s_registry.create();
		try
		{
			auto entity = std::unique_ptr<Entity>(new Entity(id, runtimeId, registryEntity, name));
			entity->AddComponent<Transform>();
			// Make the entity visible to handle lookup once its Transform is attached:
			s_entities.emplace(id, std::move(entity));
		}
		catch (...)
		{
			// Release the EnTT key and any components created before the failure:
			s_registry.destroy(registryEntity);
			throw;
		}

		return EntityHandle(id, runtimeId);
	}
	EntityHandle World::FindEntity(EntityId id)
	{
		auto it = s_entities.find(id);
		return it != s_entities.end() ? EntityHandle(id, it->second->m_runtimeId) : EntityHandle();
	}
	std::vector<EntityHandle> World::FindEntities(const std::string& name)
	{
		std::vector<EntityHandle> matches;
		for (const auto& [id, entity] : s_entities)
		{
			if (entity->GetName() == name)
				matches.push_back(EntityHandle(id, entity->m_runtimeId));
		}
		return matches;
	}
	std::vector<EntityHandle> World::GetEntities()
	{
		std::vector<EntityHandle> handles;
		handles.reserve(s_entities.size());
		for (const auto& [id, entity] : s_entities)
			handles.push_back(EntityHandle(id, entity->m_runtimeId));
		return handles;
	}
	uint32_t World::GetEntityCount()
	{
		return static_cast<uint32_t>(s_entities.size());
	}
	void World::DestroyEntity(EntityId id)
	{
		auto it = s_entities.find(id);
		if (it == s_entities.end())
			return;

		// Invalidate handles while keeping the Entity object alive through component teardown:
		auto entity = std::move(it->second);
		s_entities.erase(it);
		if (s_activeCameraEntity == EntityHandle(id, entity->m_runtimeId))
			SetActiveCamera(nullptr);
		s_registry.destroy(entity->m_registryEntry);
	}



	// Private Methods:
	// Management:
	void World::Init()
	{
		Clear();
	}
	void World::Clear()
	{
		s_activeCameraEntity = EntityHandle();
		// Clear the component pools while their owning entities still exist:
		for (auto& destroyCallback : ComponentRegistry::GetDestructionRegistry())
			destroyCallback(s_registry);
		s_registry.clear();
		s_entities.clear();
		s_bounds = emberMath::Bounds();
	}



	// ECS callbacks:
	void World::Start()
	{
		for (auto& startCallback : ComponentRegistry::GetStartRegistry())
			startCallback(s_registry);
	}
	void World::EarlyUpdate()
	{
		UpdateBounds();
		for (auto& earlyUpdateCallback : ComponentRegistry::GetEarlyUpdateRegistry())
			earlyUpdateCallback(s_registry);
	}
	void World::FixedUpdate()
	{
		for (auto& fixedUpdateCallback : ComponentRegistry::GetFixedUpdateRegistry())
			fixedUpdateCallback(s_registry);
	}
	void World::Update()
	{
		for (auto& updateCallback : ComponentRegistry::GetUpdateRegistry())
			updateCallback(s_registry);
	}
	void World::LateUpdate()
	{
		for (auto& lateUpdateCallback : ComponentRegistry::GetLateUpdateRegistry())
			lateUpdateCallback(s_registry);
	}



	// Registry:
	entt::registry& World::GetRegistry()
	{
		return s_registry;
	}
	Entity* World::ResolveEntity(EntityId entityId, uint64_t runtimeId)
	{
		auto it = s_entities.find(entityId);
		return it != s_entities.end() && it->second->m_runtimeId == runtimeId ? it->second.get() : nullptr;
	}



	// Render bounds:
	void World::UpdateBounds()
	{
		bool hasBounds = false;
		auto renderers = s_registry.view<MeshRenderer>();
		for (entt::entity registryEntity : renderers)
		{
			// Grab all active meshRenderer components:
			MeshRenderer& meshRenderer = renderers.get<MeshRenderer>(registryEntity);
			if (!meshRenderer.GetIsActive() || !meshRenderer.HasMesh())
				continue;

			// Seed the union from the first mesh's bounds:
			if (!hasBounds)
			{
				s_bounds = meshRenderer.GetWorldBounds();
				hasBounds = true;
			}
			else
				s_bounds.Encapsulate(meshRenderer.GetWorldBounds());
		}
		if (!hasBounds)
			s_bounds = emberMath::Bounds();
	}
}