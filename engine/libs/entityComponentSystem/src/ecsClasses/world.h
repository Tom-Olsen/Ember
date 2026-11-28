#pragma once
#include "entityHandle.h"
#include <entt/entity/fwd.hpp>
#include <stdint.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



// Forward declarations:
namespace emberApplication
{
	class Application;
}
namespace emberMath
{
	struct Bounds;
}



namespace emberEcs
{
	// Forward declarations:
	class Camera;
	class Entity;



	class World
	{
		// Friends:
		friend class Entity;
		friend class EntityHandle;
		friend class emberApplication::Application;

	private: // Members:
		static std::unordered_map<EntityId, std::unique_ptr<Entity>> s_entities;	// owns the live Entity objects that handles resolve to.
		static entt::registry s_registry;			// stores components under their owning entity's EnTT key.
		static EntityId s_nextEntityId;				// supplies the next stable ID assigned to a new entity.
		static uint64_t s_nextRuntimeId;			// gives reloaded entities a fresh runtime ID so old handles stay invalid even when a saved EntityId is reused.
		static EntityHandle s_activeCameraEntity;	// entity that owns the selected camera.
		static emberMath::Bounds s_bounds;			// world-space bounds of active mesh renderers with meshes.

	public: // Methods:
		// Setters:
		static void SetActiveCamera(Camera* pCamera);

		// Getters:
		static Camera* GetActiveCamera();
		static emberMath::Bounds GetBounds();

		// Entity management:
		static EntityHandle CreateEntity(const std::string& name);
		static EntityHandle FindEntity(EntityId id);
		static std::vector<EntityHandle> FindEntities(const std::string& name);
		static std::vector<EntityHandle> GetEntities();
		static uint32_t GetEntityCount();
		static void DestroyEntity(EntityId id);

	private: // Methods:
		// Constructor:
		World() = delete;

		// Management:
		static void Init();
		static void Clear();

		// ECS callbacks:
		static void Start();
		static void EarlyUpdate();
		static void FixedUpdate();
		static void Update();
		static void LateUpdate();

		// Registry:
		static entt::registry& GetRegistry();
		static Entity* ResolveEntity(EntityId entityId, uint64_t runtimeId);	// returns a borrowed pointer only when both IDs match a live entity.

		// Render bounds:
		static void UpdateBounds(); // combines the world-space bounds of active mesh renderers that have a mesh.
	};
}