
# Entity Component System

## Introduction
Ember's entity component system is the backbone of the scripting API,
which allows you to attach instances of custom `Component` classes to an `Entity`.

Ember has one [world](world.md), exposed through the static `World` class.
It is the orchestrating container which holds entities and the component registry.
Every frame, it loops over all components in the registry and invokes their [frame execution callbacks](#frame-execution-callbacks).

Entities are 'containers' for components, although the components actually reside in the world's registry.
Entities can be attached to other entities, forming a hierarchical tree of parent-child relationships.
The same type of component can be attached to an entity multiple times, and each component instance is differentiated by an ID.

## IDs and handles
The world owns its entity objects and attached component instances.
`EntityHandle` and `ComponentHandle<T>` are copyable, non-owning references to specific live instances.
`Get()` resolves a valid handle to a borrowed `Entity*` or `T*`.
Methods that return handles can return an invalid handle. Check `IsValid()` before calling `Get()` when you are unsure whether the instance exists.

Handles stop resolving when their instance is queued for destruction. `World::Clear()` invalidates all remaining handles. `EntityId` and `ComponentId` identify instances in saved data. After loading, the ECS can resolve those IDs to fresh handles. The old handles stay expired.

## Active State
Entities and components have a `isEnabled` boolean flag, which is for bookkeeping and can be queried via `GetIsEnabled()`.
An entity is active when it and all its ancestors are enabled.
A component is active when it is enabled and its owning entity is active.
Entities and components expose this effective state through `GetIsActive()`, and it determines eligibility for [frame execution callbacks](#frame-execution-callbacks).
Component activation is deferred, meaning if a component gets enabled during one frame its callbacks will not be called until the next frame.

## Frame Execution Callbacks
The frame execution callbacks are special methods that each component provides
and that you can override to achieve the desired behaviour.
Every frame, the world invokes these callbacks in exactly this order:

	OnEnable()
	Start()
	EarlyUpdate()
	FixedUpdate()
	Update()
	LateUpdate()
	OnDisable()
	OnDestroy()

OnCreate() is special and lives outside the frame execution order,
running immediately after the component is attached to an entity, even if GetIsActive() == false, serving as an initialization hook.

Note that OnEnable() is deferred to the next frame, analogous to how activity is handeled.
A component enabled in one frame will not participate in that frame's frame execution but will be deferred to the next frame.
The only frame execution callback that is exempt from this is OnDestroy(), as it runs for all components, active or not.

### Callbacks

| Callback {: .nowrap-first-column } | Explanation |
| --- | --- |
| `OnEnable()`    | Runs on all frames before which the component toggles from `GetIsActive() == false` to `true`. A component that is active when created also counts as transitioning from `GetIsActive() == false` to `true`. |
| `Start()`       | Runs once, in the first frame in which the component's `GetIsActive()` returns `true`, immediately before the component’s first scheduled update. |
| `EarlyUpdate()` | Runs every frame if `GetIsActive() == true`. |
| `FixedUpdate()` | Runs every physics frame if `GetIsActive() == true`. |
| `Update()`      | Runs every frame if `GetIsActive() == true`. |
| `LateUpdate()`  | Runs every frame if `GetIsActive() == true`. |
| `OnDisable()`   | Runs on all frames in which the component toggles from `GetIsActive() == true` to `false`. Components disabled in the middle of a frame do not receive their remaining update callbacks. |
| `OnDestroy()`   | Runs at the very end of the frame after `Renderer::RenderFrame()` returns. Components queued for destruction earlier in the frame do not receive their remaining update callbacks. Removal hides the component from lookup immediately, while an already-running callback can finish before teardown.<br><br>Component removal, entity destruction, and `World::Clear()` during application shutdown all use the same cleanup path. The ECS calls `OnDestroy()` exactly once per affected component, immediately before its C++ destructor, even if it was never active. Repeated destruction requests are ignored. |


## Examples
Here you can find example usecases for the public native (`C++`) API:
```cpp
EntityHandle guard = World::CreateEntity("Guard");
ComponentHandle<Health> health = guard.Get()->AddComponent<Health>(100);
health.Get()->ApplyDamage(25);
```


## Extra Info
The four pages are enough to start a native ECS implementation, but I wouldn’t hand them to a fresh Codex chat with “implement everything” yet. They capture the main design decisions: one static World, entity-only hierarchy, multiple components of the same type, persistent IDs separate from runtime handles, Get() with IsValid(), and the lifecycle from OnCreate() through deferred OnDestroy(). [ECS overview](/home/tom/Documents/C++/Ember/website/content/documentation/entity-component-system/index.md) · [Entity API](/home/tom/Documents/C++/Ember/website/content/documentation/entity-component-system/entity.md)

I’d settle these points before the handoff:
1. Owner access during OnDestroy(). component.md guarantees access to the owner and its Transform, while the overview says handles expire as soon as destruction is queued. Keeping those objects alive through cleanup solves their lifetime. The docs still need to say how the callback accesses them while ordinary lookup treats them as gone. Also add OnCreate() to the component’s list of overridable methods. [Handle expiry (line 16)](/home/tom/Documents/C++/Ember/website/content/documentation/entity-component-system/index.md:16) · [Component callbacks (line 23)](/home/tom/Documents/C++/Ember/website/content/documentation/entity-component-system/component.md:23)
2. Changes made from callbacks. Explicitly allow adding components, removing components, and destroying entities during callbacks. State that additions become accessible immediately, run OnCreate() on attachment, and join regular frame execution on the next eligible frame. The implementation must keep executing objects alive and traversal safe, including when a callback adds a type the ECS hasn’t seen before. This is especially important because the current storage uses emplace<T> per entity, while the new contract allows several independent instances of T. [Mutation requirements (line 117)](/home/tom/Documents/C++/Ember/ToDoECS.txt:117) · [Current component storage (line 12)](/home/tom/Documents/C++/Ember/engine/libs/entityComponentSystem/src/ecsClasses/entity.inl:12)
3. Two API gaps. Component::GetComponent() returns one handle even though multiple components of that type can exist. Decide whether it becomes a plural lookup or uses explicit instance selection. World::GetActiveCamera() also needs a selection path. The default project currently sets the camera explicitly, so a fresh implementation needs to know how to migrate that call. [Component API (line 31)](/home/tom/Documents/C++/Ember/website/content/documentation/entity-component-system/component.md:31) · [World API](world.md#public-native-api) · [Current camera setup (line 88)](/home/tom/Documents/C++/Ember/projects/defaultProject/src/scenes/defaultScene.h:88)
Some relevant discussion is still missing from the handoff. We decided that native components stay ordinary C++ classes and the future C# layer provides the gameplay-facing API. We also treated SSR execution as renderer-owned. Loading and blueprints have a documented direction, with their APIs still open. A short scope note should put C# scripting, loading, blueprints, and renderer refactoring in later work. [Loading and blueprints (line 11)](/home/tom/Documents/C++/Ember/website/content/documentation/entity-component-system/entity.md:11) · [Current SSR dispatch (line 39)](/home/tom/Documents/C++/Ember/engine/libs/entityComponentSystem/src/basicComponents/ssrDispatcher.cpp:39)

For the new chat, name these four pages as the current contract. ToDoECS.txt is useful as a bug and validation checklist, but its older Start() proposal conflicts with the agreed OnCreate() and first-update split. Once the three items above are resolved, I’d hand off the native ECS together with migration of the existing application, editor, and project callers. [Older lifecycle notes (line 117)](/home/tom/Documents/C++/Ember/ToDoECS.txt:117) · [Validation checklist (line 423)](/home/tom/Documents/C++/Ember/ToDoECS.txt:423)