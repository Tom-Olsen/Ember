# Entity
The `Entity` class is a basic container for components and child entities, forming a basic ancestral hierarchy.
Entities are differentiated by an `EntityId`, their names are editable labels and may repeat.
[`EntityHandle`](#entityhandle) is the copyable reference to an entity.

Creation, lookup, and destruction go through the static [Scene](scene.md#public-native-api) API.
Destroying an entity immediately removes it from lookup and prevents its components from receiving further updates.
The application processes queued destruction after `Renderer::RenderFrame()` returns,
any [frame execution callbacks](index.md#frame-execution-callbacks) already running will finish.

The ECS can load and unload an entity together with its descendants. (API missing)
Unloading removes an entity and all its descendants from the live ECS, invalidating their runtime handles and removing them from frame execution.
The scene keeps a reference to the saved data so it can reload those entities with the same EntityId values and fresh handles.

Any entity can be saved as a blueprint, turning it and its descendants to a reusable linked building block.
Modifications to the blueprint apply to all instanced of it.
Any blueprint instance can be detached from it, turning that instance and its descendants into an independent entity. (API missing)

## EntityHandle
An `EntityHandle` gives borrowed access to an entity. Copies refer to the same instance, and a default-constructed handle is invalid.
If you are unsure whether the entity exists, check `IsValid()` before calling `Get()`.

| Methods {: .nowrap-first-column } | Explanation |
| --- | --- |
| `bool IsValid() const` | Checks whether the handle resolves to a live entity. |
| `Entity* Get() const` | Returns a borrowed pointer to the entity. Requires a valid handle. |
| `operator==`, `operator!=` | Compare the runtime identities held by two handles. |

## Transform
Each entity always comes with exactly one `Transform` component, which defines its position, rotation, and scale in its parents space.
An entity with no parent has its tranform in world space; an entity with a parent defines its transform relative to the parent.
The transform component is an exception to the multiple-component tule. Any entity can only ever have a single transform component, and it can't be removed.

## Components
Besides the required transform component you can attach any amount of components to an entity, including multiple instances of the same component class.

## Public native API
| Getters {: .nowrap-first-column } | Explanation |
| --- | --- |
| `bool GetIsEnabled()`                                 | Is entity enabled?. |
| `bool GetIsActive()`                                  | Is entity and all its ancestors enabled? See [Active State](index.md#active-state). |
| `std::string GetName()`                               | Entity name. |
| `EntityId GetId()`                                    | EntityId of the entity. |
| `EntityHandle GetParent()`                            | Parent entityHandle, invalid if no parent exists. |
| `std::vector<EntityHandle>`<br>`GetChildren()`        | Vector of all childrens entityHandles, empty if no children exist. |
| `ComponentHandle<Transform>`<br>`GetTransform()`      | Transform of the entity relative to parents local space, always exists. |

| Setters {: .nowrap-first-column } | Explanation |
| --- | --- |
| `void SetIsEnabled(bool)`                           | Enables/disabled entity. |
| `void SetName(std::string)`                         | Renames entity, entityId and entityHandles stay valid. |
| `void SetParent(EntityHandle)`                      | Sets a parent. Entities world transform doesn't change, transform gets updated into parents local space. |
| `void SetChild(EntityHandle)`                       | Sets a child. Child entities world transform doesn't change, transform gets updated into parents local space. |
| `void SetChildren`<br>`(std::vector<EntityHandle>)` | Sets multiple children by appending them to existing children. Same behaviour as SetChild(EntityHandle). |
| `void Destroy()`                                    | Queues entity for destruction, children get rebound to its parent, or to scene if no parent exists. Children keep their current world transform. |
| `void DestroyAll()`                                 | Queues entity and all its descendents for destruction. |

| Components {: .nowrap-first-column } | Explanation |
| --- | --- |
| `bool HasComponent<T>(...)`                               | Entity has component of type T attached to it? |
| `std::vector<ComponentHandle<T>>`<br>`GetComponents<T>()` | Vector of all componentHandle<T> intances attached to the entity, empty if no T attached to entity. |
| `ComponentHandle<T>`<br>`AddComponent<T>(...)`            | Creates new component of type T wtih constructor arguments `...`, attaches it to the entity and returns a componentHandle<T> to that component. |
| `ComponentHandle<T>`<br>`FindComponent<T>(ComponentId)`   | Retrieves componentHandle<T> by componentId. Needed to resolve a componentID back to a handle. |
| `void RemoveComponent`<br>`(ComponentHandle<T>)`          | Detaches component from entity and queus its destruction. |