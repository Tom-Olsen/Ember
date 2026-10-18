# Preview

On this page you can find some highlights of renderer techniques implemented in ember.


## Screen-space reflections

Ember supports a screen space compute stage, which runs on the graphics queue right after the deferred lighting and before the forward opaque stage.
This allows for the implementation of screen space techniques, such as screen space reflections, which allow geometry to reflect any of the visibly drawn geometry and the full skybox.

<figure class="ssr-comparison" data-ssr-comparison>
  <div class="ssr-comparison__summary">
    <div>
      <span class="ssr-comparison__label">SSR off</span>
      <span class="ssr-comparison__fps">710.0 <span>FPS</span></span>
    </div>
    <button class="ssr-comparison__fullscreen" type="button" data-ssr-fullscreen aria-label="Enter fullscreen" aria-pressed="false" title="Enter fullscreen">
      <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M8 3H3v5m13-5h5v5M3 16v5h5m13-5v5h-5"/></svg>
      <span data-ssr-fullscreen-label>Fullscreen</span>
    </button>
    <div class="ssr-comparison__selected" aria-live="polite" aria-atomic="true">
      <span class="ssr-comparison__label" data-ssr-name>Full resolution</span>
      <span class="ssr-comparison__fps"><span data-ssr-fps>194.6</span> <span>FPS</span></span>
    </div>
  </div>
  <div class="ssr-comparison__viewport">
    <img class="ssr-comparison__image" data-ssr-image src="ssrFullResolution.png" width="3840" height="2080" loading="lazy" decoding="async" alt="Scene with full-resolution screen-space reflections enabled">
    <img class="ssr-comparison__image ssr-comparison__baseline" src="ssrOff.png" width="3840" height="2080" loading="lazy" decoding="async" alt="The same scene with screen-space reflections disabled">
    <div class="ssr-comparison__divider" aria-hidden="true">
      <span class="ssr-comparison__handle">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"><path d="m8 7-5 5 5 5m8-10 5 5-5 5"/></svg>
      </span>
    </div>
    <input class="ssr-comparison__slider" type="range" min="0" max="100" step="0.1" value="50" aria-label="Screenshot divider position" aria-valuetext="50% SSR off, 50% Full resolution" aria-describedby="ssr-comparison-help">
  </div>
  <fieldset class="ssr-comparison__modes">
    <legend>Right image</legend>
    <label>
      <input type="radio" name="ssr-mode" value="full-resolution" checked data-image="ssrFullResolution.png" data-name="Full resolution" data-fps="194.6">
      <span class="ssr-comparison__mode"><span>Full Resolution</span><span class="ssr-comparison__mode-fps">194.6 FPS</span></span>
    </label>
    <label>
      <input type="radio" name="ssr-mode" value="reflection-map" data-image="ssrReflectionMap.png" data-name="Reflection map" data-fps="322.9">
      <span class="ssr-comparison__mode"><span>Reflection Map</span><span class="ssr-comparison__mode-fps">322.9 FPS</span></span>
    </label>
    <label>
      <input type="radio" name="ssr-mode" value="reflection-uv-map" data-image="ssrReflectionUvMap.png" data-name="Reflection UV map" data-fps="317.2">
      <span class="ssr-comparison__mode"><span>Reflection UV Map</span><span class="ssr-comparison__mode-fps">317.2 FPS</span></span>
    </label>
  </fieldset>
  <div class="ssr-comparison__descriptions">
    <p class="ssr-comparison__description ssr-comparison__description--active" data-ssr-description="full-resolution">
	Reflections are traced and applied at the scene's full resolution.</p>
    <p class="ssr-comparison__description" data-ssr-description="reflection-map" aria-hidden="true">
	Reflected colors are stored in a separate reflection map at half resolution.
	A second pass composites the reflections into the scene.
	Half resolution sampling leads to reflections beeig at half resoluton, which is noticable on flat reflective surfaces.</p>
    <p class="ssr-comparison__description" data-ssr-description="reflection-uv-map" aria-hidden="true">
	Reflection hit coordinates are stored in a uv map at half resolution.
	A second pass samples the scene color at those coordinates and composites the reflections into the scene.
	Using the sampled uvs to access the scene color texture resolves the reflections at full resolution, but can lead to artifacts on geometry boundaries.</p>
  </div>
  <p class="ssr-comparison__status" data-ssr-status role="alert" hidden></p>
</figure>

The 'Reflection Map' and 'Reflection UV Map' achieve very similar results to the 'Full Resolution' approach with twice as many fps.<br>
See the [rendering pipeline](../vulkan-renderer/rendering-pipeline.md) for how rendering and compute passes fit together.

## Fluid dynamics

The fluid dynamics project runs a three-dimensional smoothed-particle hydrodynamics (SPH) simulation on the GPU.
These recordings show its particle, water, and cloud rendering modes.

### Particle rendering (300fps)

Each particle is rendered as a camera facing billboard quad via instanced rendering, reading its position and color (density) from the sph gpu buffers.
A custom shader turns the quads into 3d looking spheres and updates their depth according to a sphere.

<video controls playsinline preload="none" poster="particleShader.jpg" width="3840" height="2086" style="width: 100%; height: auto;" aria-label="SPH fluid simulation rendered as individual particles">
  <source src="particleShader.webm" type='video/webm; codecs="av01.0.13M.10"'>
  Your browser does not support embedded WebM video. <a href="particleShader.webm">Download the particle rendering video</a>.
</video>

### Water rendering (80fps - 100fps)

The water shader is a ray marching algorithm which reconstructs the fluids surface via marching through the fluid simulation volume and detecting a density threshold.
At each detected surface, the ray splits into reflected and refracted rays, up to four times following Snell's law, $n_0 \sin(\theta_0) = n_1 \sin(\theta_1)$.
The water shader is implemented in the forward transparent render stage, giving it access to the scene color with all opaque geometry already rendered.
This allows reflecting and refracting the visible scene, similar to screen space reflections, and the full skybox, which also acts as a fallback for any ray that misses all geometry.

<video controls playsinline preload="none" poster="waterShader.jpg" width="3840" height="2086" style="width: 100%; height: auto;" aria-label="SPH fluid simulation rendered as reflective and refractive water">
  <source src="waterShader.webm" type='video/webm; codecs="av01.0.13M.10"'>
  Your browser does not support embedded WebM video. <a href="waterShader.webm">Download the water rendering video</a>.
</video>

### Volumetric rendering (350fps)

Cloud mode uses a volumetric shader which ray marches through the volume and accumulates light through scattering and absorption with every step.
The directional light similarly renders a optical depth 3d texture which is used for volumetric lighting, allowing the fluid to cast shadows on itself.

<video controls playsinline preload="none" poster="volumetricShader.jpg" width="3840" height="2086" style="width: 100%; height: auto;" aria-label="SPH fluid simulation rendered as a cloud-like volume">
  <source src="volumetricShader.webm" type='video/webm; codecs="av01.0.13M.10"'>
  Your browser does not support embedded WebM video. <a href="volumetricShader.webm">Download the volumetric rendering video</a>.
</video>