# Scripting (C#)

## Notes

- The C++ ECS owns the world and schedules callbacks. C# provides the gameplay API.
- Ember provides C# wrappers for `World`, `Entity`, and built-in components such as `Transform`. The wrappers use opaque runtime handles internally.
- Write custom components as C# classes derived from `Component`, with their own fields, methods, and callback overrides. `Health health = guard.AddComponent<Health>();` returns the attached C# instance.
- Expose entity creation and lookup, hierarchy and transform access, and add, find, and remove operations for components. Support multiple components of the same type and removal by instance.
- Cached references can expire when an entity or component is removed. Use saved IDs when a reference needs to survive unloading and reloading.
- The native engine hosts .NET and invokes managed callbacks at the same points as native callbacks. The C# wrappers call a C-compatible engine API for operations on native objects.