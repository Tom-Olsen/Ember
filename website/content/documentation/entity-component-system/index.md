
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

Handles stop resolving when their instance is queued for destruction. `World::Clear()` invalidates all remaining handles.
`EntityId` and `ComponentId` identify live and saved instances.
After loading an entity, the ECS can resolve those IDs to fresh handles. The old handles stay expired.

## Active State
Entities and components have an `isEnabled` boolean flag, which is for bookkeeping and can be queried via `GetIsEnabled()`.
An entity is active when it and all its ancestors are enabled.
A component is active when it is enabled and its owning entity is active.
Entities and components expose this effective state through `GetIsActive()`, and it determines eligibility for [frame execution callbacks](#frame-execution-callbacks).

## Trigger Callbacks
Each component exposes these virtual callback methods which get called on their respective triggers:

| Callback {: .nowrap-first-column } | Explanation |
| --- | --- |
| `OnCreate()`  | Invoked immediately after the component is attached to an entity, even if `GetIsActive() == false`, serving as the first initialization hook. |
| `OnEnable()`  | Invoked immediately on a `GetIsActive()` toggle from `false` to `true`. If the callback is triggered in an update phase, the enabled component will not participate in the current phase, but later phases will contain it. |
| `OnDisable()` | Invoked immediately on a `GetIsActive()` toggle from `true` to `false`. If the callback is triggered in an update phase, the disabled component will not participate in the current phase if its update callback was not already invoked. Destroying an entity/component automatically disables it first. |
| `Start()` | Scheduled on the component’s first `OnEnable()`. The world invokes it once before the first `EarlyUpdate()`, `FixedUpdate()`, `Update()`, or `LateUpdate()` the component is eligible to join, whichever happens first. A component that never gets enabled will trigger its `OnDestroy()` without ever running `Start()`. `Start()` serves as the second initialization hook. |

## Frame Execution Callbacks
The frame execution callbacks are special virtual methods that each component provides
and that you can override to achieve the desired behaviour.
Every frame, the world invokes these callbacks in exactly this order:

| Callback {: .nowrap-first-column } | Explanation |
| --- | --- |
| `EarlyUpdate()` | Runs every frame if `GetIsActive() == true`. |
| `FixedUpdate()` | Runs every physics tick if `GetIsActive() == true`. |
| `Update()`      | Runs every frame if `GetIsActive() == true`. |
| `LateUpdate()`  | Runs every frame if `GetIsActive() == true`. |
| `OnDestroy()`   | Runs at the very end of the frame after `Renderer::RenderFrame()` returns. Components queued for destruction earlier in the frame do not receive their remaining update callbacks. Removal hides the component from lookup immediately, while an already-running callback can finish before teardown. <br><br> Component removal, entity destruction, and `World::Clear()` during application shutdown all use the same cleanup path. The ECS calls `OnDestroy()` exactly once per affected component, immediately before its C++ destructor, even if it was never active. Repeated destruction requests are ignored. |

A component enabled in the middle of a frame will participate in the remaining frame execution callbacks.
A component disabled/destroyed in the middle of a frame will finish all invoked callbacks (e.g. running `Update()`) but not the following ones (e.g. `LateUpdate()`).
`OnDestroy()` is exempt from this, as it runs at the end of the frame as a deferred destruction callback for all components that get destroyed, active or not.


## Examples
Here you can find example use cases for the public native (`C++`) API:

Entity creation and attaching a component:
```cpp
EntityHandle guard = World::CreateEntity("Guard");
ComponentHandle<Health> health = guard.Get()->AddComponent<Health>(100);
health.Get()->ApplyDamage(25);
```

Here are some examples of how a component's callbacks might be triggered (pseudocode):

Component created in an enabled state and destroyed mid-frame:
```text
ComponentHandle<T> comp = entity->AddComponent<T>(true) (enabled)
	OnCreate()
	OnEnable()
Frame gets executed
	Start()
	EarlyUpdate()
	FixedUpdate()
	Update()
	LateUpdate()
comp gets destroyed in EarlyUpdate() by another component
	OnDisable() runs immediately
	EarlyUpdate() finishes if already running
	OnDestroy()
```

Component created in a disabled state and enabled mid-frame:
```text
ComponentHandle<T> comp = entity->AddComponent<T>(false) (disabled)
	OnCreate()
comp gets enabled in EarlyUpdate() by another component
	OnEnable()
	Start()
	Update()
	LateUpdate()
```

A component disables itself in its own Update() callback.
This is dangerous, as OnDisable() can change state that Update() relies on, breaking it mid-execution:
```text
ComponentHandle<T> comp = entity->AddComponent<T>(true) (enabled)
	OnCreate()
	OnEnable()
Frame gets executed
	Start()
	EarlyUpdate()
comp disables itself in Update()
	Update()
	OnDisable() runs immediately
	Update() finishes if already running
```