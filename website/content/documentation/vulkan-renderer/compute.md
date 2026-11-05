# Compute
The compute subsystem supports two execution models: work synchronized with a rendered frame and work submitted independently of the render graph. `Dispatch*` APIs collect work on the CPU, stage `Record*` methods write Vulkan commands into command buffers, and submission sends those command buffers to the GPU.

## Render-graph compute
Five queues place compute work at defined points in the frame:

| Queue | Position and purpose |
| --- | --- |
| Pre-render | Runs after resource updates and before the main graphics branches consume their inputs. |
| Post-outline | Runs after outline rendering and can prepare results needed later in the frame. |
| Post-geometry | Runs after deferred geometry and before deferred lighting, with the G-buffer and depth available for sampling. |
| Post-lighting | Runs after deferred lighting and before forward rendering continues. |
| Post-render | Joins the post-outline and main scene branches before presentation. |

Each queue collects dispatches and explicit barriers from the engine. At frame submission, its calls become part of the frame context and are recorded by the matching compute stage. Call resources remain associated with their frame-in-flight slot until that slot is retired.

Post-geometry work runs on the graphics queue. Dispatch work through `Compute::PostGeometry::DispatchComputeShader(shader, threadCount)` or `Compute::DispatchComputeShader(ComputeType::postGeometry, shader, threadCount)`. Supply explicit thread counts. Albedo, normals, and depth are sampled through the existing frame-set textures. Surface properties can also be updated through `gbufferSurfacePropertiesStorageTexture`, an `RWTexture2D<float4>` backed by the same image as `gbufferSurfacePropertiesSampleTexture`.

The renderer transitions surface properties to `GENERAL` for this stage and restores `SHADER_READ_ONLY_OPTIMAL` before deferred lighting. All compute stages bind the same static descriptor sets. During post-geometry compute, read and write surface properties through `gbufferSurfacePropertiesStorageTexture`. Albedo, normals, and depth remain available through their read-only sampled bindings. After the surface-properties layout is restored, later stages can use `gbufferSurfacePropertiesSampleTexture` for filtered sampling or integer-coordinate `Load` operations.

Renderer-owned effects use the same mechanism. For example, the outline path uses graphics work to create a mask and compute work to expand it before final presentation.

## Asynchronous compute
Asynchronous compute sessions are separate from the render graph. A session collects compute dispatches, buffer copies, and barriers through the `Dispatch*` APIs. `SubmitComputeSession()` records those calls into its command buffer and submits it to the GPU. `SubmitComputeSessionAndWait()` also waits for completion. The session exposes polling and waiting for completion.

This model is intended for workloads whose lifetime is controlled explicitly by a caller rather than by the render-frame sequence. Session IDs and a fixed session pool allow completed command resources to be collected and reused.

Physics work uses `Compute::Physics::BeginSession()` and `EndSession()` to collect and submit a workload. `GetActiveSessionID()` returns the current physics session ID while a session is active.

## Shared compute resources
Both execution models use the compute-shader manager, reflected descriptor bindings, compute pipelines, per-dispatch descriptor sets, and push constants. The distinction is scheduling and lifetime: render-graph queues participate in frame dependencies, while asynchronous sessions manage their own completion.