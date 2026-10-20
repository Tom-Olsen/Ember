# Rendering Pipeline
The graphics pipeline combines deferred rendering for the main opaque scene with forward rendering for objects that need a forward path. Separate branches generate shadows, outlines, and editor gizmos before the final image is presented.

## Graphics stages
| Stage | Responsibility |
| --- | --- |
| Resource update | Uploads queued mesh changes before rendering reads them. |
| Gizmo | Renders editor and debug geometry into a separate gizmo target. |
| Outline | Produces the object-selection mask used by the outline compute work. |
| Shadow | Renders the active shadow-casting lights into layers of the shadow-map array. |
| Deferred geometry | Writes material and geometry data to the G-buffer and scene-depth targets. |
| Deferred lighting | Combines G-buffer, depth, light, and shadow data into scene color. |
| Forward opaque | Adds opaque materials assigned to the forward path. |
| Forward transparent | Draws sorted transparent geometry after opaque scene rendering. |
| Present | Publishes the final scene color and either composites it to the swapchain or lets the GUI present it in the editor layout. |

Compute stages are interleaved with these graphics stages and are summarized in [Compute](compute.md).

## Execution layers
The backend separates four related responsibilities:

- **Draw-call records** capture the mesh, material, transform, per-call descriptors, render state, and instance count needed for a submission.
- **Stages** consume a category of frame work and record its commands into the command buffer assigned to that stage.
- **Render passes** define the attachments and framebuffers used by a graphics stage.
- **Pipelines** package the shader and fixed-function state required to execute a draw or dispatch.

Materials connect shader state to the pipelines used by the deferred, forward, shadow, gizmo, outline, and presentation paths. Draw calls are grouped and sorted before recording so a stage can reduce unnecessary state changes and order transparent work correctly.

## Render targets
Render-target ownership is centralized rather than distributed across individual stages. The target set includes scene color, scene depth, G-buffer attachments, shadow maps, outline intermediates, and gizmo color/depth images. Most targets have one instance per frame-in-flight slot; swapchain images remain presentation resources owned by the Vulkan context.