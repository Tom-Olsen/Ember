
# Entity Component System:

## Introduction
Ember's entity component system is the backbone of the scripting API,
which allows you to add custom code as **components** to any **entity** in your **scene**.

The scene is the orchestrating container which holds entities and the component registry.
Every frame, it loops over all components in the registry and invokes their [frame execution callbacks](#frame-execution-callbacks).

Entities are 'containers' for components, although the components actually reside in the scene's registry.
Entities can be attached to other entities, forming a hierarchical tree of parent-child relationships.
The same type of component can be attached to an entity multiple times, and each component instance is differentiated by an ID.

## Active State
Each scene, entity, and component has its own **isEnabled** boolean flag, which is for bookkeeping and can be queried with x.GetIsEnabled().
Entities and components also provide x.GetIsActive(), which also checks the owning entity, its ancestors, and the scene hierarchy,
meaning a component or entity can be enabled but inactive.
The active state determines whether [frame execution callbacks](#frame-execution-callbacks) run.
Component activation is deferred, meaning if a component gets enabled during one frame its callbacks will not be called until the next frame.

## Frame Execution Callbacks
The frame execution callbacks are special methods that each component provides
and that you can override to achieve the desired behaviour.
Every frame, the scene invokes these callbacks in exactly this order:

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

### Callback Timing

| Callback {: .ecs-callback-column } | Explanation |
| --- | --- |
| `OnEnable()` | Runs on all frames before which the component toggles from `GetIsActive() == false` to `true`. A component that is active when created also counts as transitioning from `GetIsActive() == false` to `true`. |
| `Start()` | Runs once, in the first frame in which the component's `GetIsActive()` returns `true`, immediately before the component’s first scheduled update. |
| `EarlyUpdate()` | Runs every frame if `GetIsActive() == true`. |
| `FixedUpdate()` | Runs every physics frame if `GetIsActive() == true`. |
| `Update()` | Runs every frame if `GetIsActive() == true`. |
| `LateUpdate()` | Runs every frame if `GetIsActive() == true`. |
| `OnDisable()` | Runs on all frames in which the component toggles from `GetIsActive() == true` to `false`. Components disabled in the middle of a frame do not receive their remaining update callbacks. |
| `OnDestroy()` | Runs at the very end of the frame after `Renderer::RenderFrame()` returns. Components queued for destruction earlier in the frame do not receive their remaining update callbacks. Removal hides the component from lookup immediately, while an already-running callback can finish before teardown.<br><br>Component removal, entity destruction, scene unload or destruction, and application shutdown all use the same cleanup path. The ECS calls `OnDestroy()` exactly once per affected component, immediately before its C++ destructor, even if it was never active. Repeated destruction requests are ignored. |