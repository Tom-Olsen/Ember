# Entity
The `Entity` class is a basic container for components and child entities, forming a basic ancestral hierarchy.
Entities are differentiated by an `EntityId`, and their names are editable labels and may repeat.
[`EntityHandle`](#entityhandle) is the copyable reference to an entity.

Creation, lookup, and destruction go through the static [World](world.md#public-native-api) API.
Destroying an entity immediately removes it from lookup and prevents its components from receiving further updates.
The application processes queued destruction at the end of the frame,
and all invoked [update callbacks](index.md#update-callbacks) will finish their execution.

The ECS can load and unload an entity together with its descendants. (API missing)
Unloading removes an entity and all its descendants from the live ECS, invalidating their runtime handles and removing them from frame execution.
The world keeps a reference to the saved data so it can reload those entities with the same EntityId values and fresh handles.

Any entity can be saved as a blueprint, turning it and its descendants into a reusable linked building block.
Modifications to the blueprint apply to all instances of it.
Any blueprint instance can be detached, turning that instance and its descendants into an independent entity. (API missing)

## EntityHandle
An `EntityHandle` gives borrowed access to an entity.
Copies refer to the same instance, and a default-constructed handle is invalid.
If you are unsure whether the entity exists, check `IsValid()` before calling `Get()`.

| Methods {: .nowrap-first-column } | Explanation |
| --- | --- |
| `bool IsValid() const`     | Checks whether the handle resolves to a live entity. |
| `Entity* Get() const`      | Returns a borrowed pointer to the entity. Requires a valid handle. |
| `operator==`, `operator!=` | Compare the runtime identities held by two handles. |

## Transform
Each entity always comes with exactly one `Transform` component, which defines its position, rotation, and scale in its parent's space.
The entity class holds a reference to its transform, ensuring access to it even if the transform component is queued for destruction and thus all related handles have been invalidated.
Root entities have transforms in world space, the shared coordinate space of the `World`. Child entities have transforms relative to their parent.
The transform component is an exception to the multiple-component rule. Any entity can only ever have a single transform component, and it can't be removed.

## Components
Besides the required transform component, you can attach any number of components to an entity, including multiple instances of the same component class.

## Public native API
| Getters {: .nowrap-first-column } | Explanation |
| --- | --- |
| `bool GetIsEnabled()`                                 | Is the entity enabled? |
| `bool GetIsActive()`                                  | Are the entity and all its ancestors enabled? See [Active State](index.md#active-state). |
| `std::string GetName()`                               | Entity name. |
| `EntityId GetId()`                                    | EntityId of the entity. |
| `EntityHandle GetParent()`                            | Parent EntityHandle, invalid for root entities. |
| `std::vector<EntityHandle>`<br>`GetChildren()`        | Vector of all children's EntityHandles, empty if no children exist. |
| `std::vector<EntityHandle>`<br>`GetAncestors()`       | Vector of all ancestors' EntityHandles, empty if no ancestors exist. |
| `ComponentHandle<Transform>`<br>`GetTransform()`      | Transform of the entity relative to the parent's local space. It always exists. |

| Setters {: .nowrap-first-column } | Explanation |
| --- | --- |
| `void SetIsEnabled(bool)`                           | Enables/disables the entity. |
| `void SetName(std::string)`                         | Renames the entity. Its EntityId and EntityHandles stay valid. |
| `void SetParent(EntityHandle)`                      | Sets a parent. The entity's world transform doesn't change, and the transform gets updated into the parent's local space. |
| `void SetChild(EntityHandle)`                       | Sets a child. The child entity's world transform doesn't change, and the transform gets updated into the parent's local space. |
| `void SetChildren`<br>`(std::vector<EntityHandle>)` | Sets multiple children by appending them to existing children. Same behaviour as SetChild(EntityHandle). |
| `void Destroy()`                                    | Queues the entity for destruction. Its children move to its parent or become root entities if it had no parent. They keep their world transforms. |
| `void DestroyAll()`                                 | Queues the entity and all its descendants for destruction. |

| Components {: .nowrap-first-column } | Explanation |
| --- | --- |
| `bool HasComponent<T>(...)`                               | Does the entity have a component of type T attached to it? |
| `ComponentHandle<T>`<br>`GetComponent<T>(uint32_t)`          | ComponentHandle attached to the entit. Index picks concrete instance of T, 0 by default. Invalid handle if no T is attached to the entity, or index out of range. |
| `std::vector<ComponentHandle<T>>`<br>`GetComponents<T>()` | Vector of all ComponentHandle instances attached to the entity, empty if no T is attached to the entity. |
| `ComponentHandle<T>`<br>`AddComponent<T>(...)`            | Creates a new component of type T with constructor arguments `...`, attaches it to the entity, and returns a ComponentHandle to that component. |
| `ComponentHandle<T>`<br>`FindComponent<T>(ComponentId)`   | Retrieves a ComponentHandle by ComponentId. Needed to resolve a ComponentId back to a handle. |
| `void RemoveComponent`<br>`(ComponentHandle<T>)`          | Detaches the component from the entity, then disables and destroys it. |

| Other Methods {: .nowrap-first-column } | Explanation |
| --- | --- |
| `void DetachChild`<br>`(EntityHandle)` | Detaches a child from this entity, turning it into a root entity. The child's world transform doesn't change. |
| `EntityHandle Copy`<br>`(std::string)` | Creates a new entity with a fresh `EntityId` and an optional new name. Copies the transform values into its required `Transform` and attaches [copies of other components that support copying](component.md#copying-components). |