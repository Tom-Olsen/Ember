# World
Ember exposes a single world through the static `World` class.
Initialization and cleanup are automatically managed by the application layer (add link later) and are not a user concern.
The world owns the ECS registry, all entities, and their attached component instances.

## Registry
The world's registry tracks which entities exist and which component instances belong to each entity.
It lets the ECS resolve an entity ID to a live entity, query its components by type,
and find the instances that need callbacks or teardown.
An entity can have several components of the same type, and the registry tracks each instance separately.

Native code (`C++`) uses [runtime handles](index.md#ids-and-handles) to access live entities and components.
The registry stays internal so the world can enforce ownership and lifetime rules consistently.

## Public native API
| Getters {: .nowrap-first-column } | Explanation |
| --- | --- |
| `ComponentHandle<Camera>`<br>`GetActiveCamera()` | Active camera used for rendering, invalid handle when none exists. |
| `Bounds GetBounds()`                             | Combined bounds of all render bounds. |
| `Bounds GetActiveBounds()`                       | Combined bounds of render bounds of active render components. |

| Entity Methods {: .nowrap-first-column } | Explanation |
| --- | --- |
| `EntityHandle CreateEntity(name)`                         | Creates a new entity. |
| `EntityHandle FindEntity(id)`                             | Finds an entity by ID. A miss returns an invalid handle. |
| `std::vector<EntityHandle>`<br>`FindEntitiesByName(name)` | Finds all entities with that name. A miss returns an empty list. |
| `std::vector<EntityHandle>`<br>`GetEntities()`            | Returns all existing entities. |
| `uint32_t GetEntityCount()`                               | Number of entities in the world. |
| `void DestroyEntity(id)`                                  | Queues destruction of the entity by ID. Its handle stops resolving immediately. Children move to the destroyed entity's parent or become root entities if it had no parent. They keep their world transforms. |
| `void DestroyEntityAll(id)`                               | Queues destruction of the entity and all its descendants by ID. All their handles stop resolving immediately. |

## Private native API
The private native API is mostly used in the application layer (add link later), which is a friend of `World`.
The application initializes the world on startup, tears it down on shutdown,
and drives the [update](index.md#update-callbacks) and [deferred](index.md#deferred-callbacks) callbacks in the correct locations during the frame.

| Management {: .nowrap-first-column } | Explanation |
| --- | --- |
| `void Init()` | Initializes all containers to an empty state. |
| `void Clear()` | Calls OnDestroy() for all components, clears the component registry, and destroys all entities. |
| `void Start()` | Dispatched before a component's first [update callback](index.md#update-callbacks). |

| Update Callbacks {: .nowrap-first-column } | Explanation |
| --- | --- |
| `void EarlyUpdate()` | Dispatched for all active components. |
| `void FixedUpdate()` | Dispatched for all active components. A frame can contain zero or more physics ticks. |
| `void Update()` | Dispatched for all active components. |
| `void LateUpdate()` | Dispatched for all active components. |

| Deferred Callbacks {: .nowrap-first-column } | Explanation |
| --- | --- |
| `void OnDestroy()` | Dispatched for all components queued for destruction, active or not. |