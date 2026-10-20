# Fluid Dynamics

The fluid dynamics project runs a three-dimensional smoothed-particle hydrodynamics (SPH) simulation on the GPU.
An async physics compute stage updates the simulation 50 times per second and publishes final results for rendering via tripple buffered state.
The scene used for the different render modes (particle/water/volumetric) uses 100.000 particles, while the last render demonstrates performance with 1.000.000 particles.
All fps numbers where recorded on an Intel i7-14900KF and an Nvidia RTX 3070Ti.

## Particle rendering (~300fps)

Each particle is rendered as a camera facing billboard quad via instanced rendering, reading its position and color (density) from the sph gpu buffers.
A custom shader turns the quads into 3d looking spheres and updates their depth accordingly.

<video controls playsinline preload="none" poster="particleShader.jpg" width="3840" height="2086" style="width: 100%; height: auto;" aria-label="SPH fluid simulation rendered as individual particles">
  <source src="particleShader.webm" type='video/webm; codecs="av01.0.13M.10"'>
  Your browser does not support embedded WebM video. <a href="particleShader.webm">Download the particle rendering video</a>.
</video>

## Water rendering (80fps - 100fps)

The water shader is a ray marching algorithm which reconstructs the fluids surface via marching through the fluid simulation volume and detecting a density threshold.
At each detected surface, the ray splits into reflected and refracted rays, up to four times following Snell's law, $n_0 \sin(\theta_0) = n_1 \sin(\theta_1)$,
and combines both paths via the Fresnel equation for unpolarized light:

$$
R = \frac{1}{2}\left[\left(\frac{n_0\cos\theta_0 - n_1\cos\theta_1}{n_0\cos\theta_0 + n_1\cos\theta_1}\right)^2 + \left(\frac{n_0\cos\theta_1 - n_1\cos\theta_0}{n_0\cos\theta_1 + n_1\cos\theta_0}\right)^2\right]
$$

The reflected and refracted weights are $R$ and $1-R$, respectively.

The water shader is implemented in the forward transparent render stage, giving it access to the scene color with all opaque geometry already rendered.
This allows reflecting and refracting the visible scene, similar to screen space reflections, and the full skybox, which also acts as a fallback for any ray that misses all geometry.

<video controls playsinline preload="none" poster="waterShader.jpg" width="3840" height="2086" style="width: 100%; height: auto;" aria-label="SPH fluid simulation rendered as reflective and refractive water">
  <source src="waterShader.webm" type='video/webm; codecs="av01.0.13M.10"'>
  Your browser does not support embedded WebM video. <a href="waterShader.webm">Download the water rendering video</a>.
</video>

## Volumetric rendering (~350fps)

Cloud mode uses a volumetric shader which ray marches through the volume and accumulates light through scattering and absorption with every step.
The directional light similarly renders a optical depth 3d texture which is used for volumetric lighting, allowing the fluid to cast shadows on itself.

<video controls playsinline preload="none" poster="volumetricShader.jpg" width="3840" height="2086" style="width: 100%; height: auto;" aria-label="SPH fluid simulation rendered as a cloud-like volume">
  <source src="volumetricShader.webm" type='video/webm; codecs="av01.0.13M.10"'>
  Your browser does not support embedded WebM video. <a href="volumetricShader.webm">Download the volumetric rendering video</a>.
</video>

## One million particles (~50fps)

Simulation with one million particles rendered in particle mode.
This is the maximum number of particles ember can currently run in real time, as it pushes the fps down to ~50, which is the update frequency of the physics system.
Any time the frame rate drops below 50 the physics compute worklaod is skipped to allow previous work to finish, resulting in the simulation slowing down.<br>
Switching to the volumetric cloud or water rendering reduces the frame rate, to around 40 and 35 fps respectively.
Pausing the simulation brings rendering performance up to around 200 fps for clouds and 100 fps for water.

<video controls playsinline preload="none" poster="1Mparticles.jpg" width="3840" height="2086" style="width: 100%; height: auto;" aria-label="SPH fluid simulation with one million particles">
  <source src="1Mparticles.webm" type='video/webm; codecs="av01.0.13M.10"'>
  Your browser does not support embedded WebM video. <a href="1Mparticles.webm">Download the one million particles video</a>.
</video>

## Explore further

Read about [compute scheduling](../../documentation/vulkan-renderer/compute.md) in Ember's documentation. The planned [SPH lesson on Sparkive](https://tom-olsen.github.io/sparkive/physics/smoothed-particle-hydrodynamics/) will cover the numerical method.

[All showcase features](../index.md)