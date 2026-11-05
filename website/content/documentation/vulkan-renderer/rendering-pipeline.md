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

## Ambient lighting and SSAO
`Renderer::SetAmbientLight(color, intensity)` sets a uniform diffuse ambient contribution using a linear RGB color. Its initial intensity is zero; the default scene enables white ambient lighting at `0.35`.

Ambient occlusion attenuates this diffuse ambient contribution. Direct lights retain their existing shadowing, and unlit materials return their albedo. Metals have no diffuse ambient contribution. Forward materials use their material AO, while deferred lighting reads AO from the surface-properties G-buffer.

`SsaoDispatcher` queues SSAO in post-geometry compute and replaces the material AO channel before deferred lighting. `SetSsaoMode()` selects `SsaoMode::off`, `SsaoMode::kernelSampled`, or `SsaoMode::bitmaskSampled`, and `SetResolutionScale()` sets the AO width and height relative to the render dimensions. The scale accepts values in `[0.1, 1]`: `1` evaluates every render pixel, while `0.5` evaluates approximately one quarter as many pixels. The editor application's **Screen Space Ambient Occlusion** window exposes these settings along with sampling, blur, and temporal reconstruction parameters.

The selected estimator writes raw occlusion into a single-channel floating-point texture at the chosen resolution, rounding each dimension up and retaining at least one texel. The default kernel-sampled mode uses a scale of `0.5` and 32 samples. Bitmask-sampled SSAO follows [Screen Space Indirect Lighting with Visibility Bitmask](https://arxiv.org/abs/2301.11376): each slice tracks 32 angular visibility sectors, treating depth samples as surfaces with a constant thickness rather than infinitely deep occluders. `SetSliceCount()`, `SetSamplesPerSide()`, and `SetSampleThickness()` control this estimator; defaults are four slices, four samples per side (32 depth fetches per pixel), and `0.2` world units of thickness. Radius, normal bias, strength, resolution, and reconstruction settings are shared between modes.

Temporal reconstruction always follows the estimator and reprojects the immediately preceding frame using the velocity G-buffer, rejects incompatible depth and normals, clamps history to the current neighborhood, and blends it with the new estimate. History weight, depth tolerances, minimum normal similarity, neighborhood radius, and seed modulo are editable. A seed modulo of `8` cycles through indices `0`–`7`; `1` keeps sampling fixed while still accumulating history during motion. Set history weight to `0` to bypass averaging. Mode, AO or render dimension, kernel sample-count, slice-count, and samples-per-side changes invalidate history. Radius, bias, thickness, and seed-modulo adjustments retain accumulated history. Call `ResetHistory()` after camera cuts.

A full-resolution reconstruction pass then applies a depth- and normal-aware blur at every scale, including `1`, applies strength, and writes the G-buffer AO channel. Blur radius and spatial offsets use AO texels: radius `1` selects a 3×3 stencil regardless of resolution, while radius `0` selects the nearest AO texel. The screen footprint grows at lower scales without increasing the stencil size. Spatial weight scale, absolute and relative depth tolerances, and normal weight exponent are editable. The source-pixel mapping and geometry weights are shared with SSR through `bilateralUpsampling.hlsli`; AO does not use SSR's roughness or reflection-confidence weights. Pixels without compatible reconstruction samples receive an AO value of one.

Call `SetSsaoMode(SsaoDispatcher::SsaoMode::off)` or disable the component with `SetIsEnabled(false)` to compare against material AO, or set ambient intensity to zero to inspect direct lighting alone.

## Execution layers
The backend separates four related responsibilities:

- **Draw-call records** capture the mesh, material, transform, per-call descriptors, render state, and instance count needed for a submission.
- **Stages** consume a category of frame work and record its commands into the command buffer assigned to that stage.
- **Render passes** define the attachments and framebuffers used by a graphics stage.
- **Pipelines** package the shader and fixed-function state required to execute a draw or dispatch.

Materials connect shader state to the pipelines used by the deferred, forward, shadow, gizmo, outline, and presentation paths. Draw calls are grouped and sorted before recording so a stage can reduce unnecessary state changes and order transparent work correctly.

## Render targets
Render-target ownership is centralized rather than distributed across individual stages. The target set includes scene color, scene depth, G-buffer attachments, shadow maps, outline intermediates, and gizmo color/depth images. Most targets have one instance per frame-in-flight slot; swapchain images remain presentation resources owned by the Vulkan context.