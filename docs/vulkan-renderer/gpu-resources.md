# GPU Resources and Descriptors
The resource subsystem turns Ember's backend-independent resource interfaces into Vulkan buffers, images, samplers, meshes, shaders, and materials. It also manages how those resources are made visible to shaders.

## Creation and ownership
The GPU resource factory is the public construction boundary. It creates buffers, textures, meshes, and per-call descriptor bindings and provides access to the material and compute-shader managers.

Within the backend, responsibilities are divided between:

- **Resource objects**, which wrap Vulkan buffers, images, views, samplers, shader modules, and related state.
- **Managers**, which own and identify material shaders, materials, and compute shaders.
- **Default resources**, which provide built-in samplers, textures, buffers, materials, and compute shaders required by renderer-owned stages.
- **Registries and handles**, which allow non-owning references to detect whether a resource still exists.
- **Pools**, which reuse command buffers, staging buffers, and per-call descriptor bindings.

## Resource groups
The backend supports several broad resource groups:

- Vertex, index, uniform, storage, and staging buffers.
- Sampled, storage, depth, render, G-buffer, and scene-color textures.
- Meshes composed from vertex and index data.
- Material and compute shaders, including their pipelines and reflected resource layouts.
- Materials and descriptor bindings that supply resources to shader declarations.

## Descriptor-set hierarchy
Descriptor sets are ordered by how broadly their data is shared and how often it changes:

| Set | Scope | Typical responsibility |
| --- | --- | --- |
| 0 | Global | Shared samplers and the shadow-map array. |
| 1 | Scene | Directional and positional light data. |
| 2 | Frame | Camera data and the scene depth, G-buffer, and scene-color targets for the current frame slot. |
| 3 | Shader or material | Resources shared by calls using one shader or material. |
| 4 | Draw or dispatch call | Resources unique to one recorded draw or compute call. |

The renderer owns the global, scene, and frame bindings. Shader/material and call bindings are built from shader reflection and supplied by the resource and compute systems.

## Allocation and updates
Vulkan Memory Allocator provides the underlying buffer and image allocations. The backend layers allocation tracking, descriptor-pool management, staging-buffer reuse, batched texture upload, and deferred destruction around those allocations. Queued mesh changes that must precede drawing enter the frame through the resource-update stage.