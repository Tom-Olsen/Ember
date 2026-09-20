Build Requirements:
To compile this project, you need to have Vulkan SDK 1.3.290.0 (or later), cmake version 3.30 (or later), and dxc
(included in vulkan on windows and some linux distros, see https://github.com/microsoft/DirectXShaderCompiler/releases for latest release and install if needed).
All other dependencies (googletest, imgui, SDL, spdlog, SPIRV-Reflect, vma) are submodules and will be compiled from source.
(Don't forget to clone recursively for the submodules: git clone --recurse-submodules <repository-url>, or after cloning do: git submodule update --init --recursive)
Note that on some linux distros further dependencies for SDL might be needed. In this case cmake will fail when trying to generate the sdl dependent targets.
Simply follow the instructions of the error message and/or ask an LLM of your choice.



Architecture:
The engine consists of a core (Ember/engine/core) and multiple backend libraries: imGuiSdlVulkan, nullGui, nullWindow, sdlWindow, vulkanRenderer (Ember/engine/backends).
The core and the backends all build into shared libraries which then get linked into multiple statically build applications (Ember/engine/applications), emberEditorApp, emberGameApp, emberGameDebugApp, emberHeadlessApp.
The difference between these applications is the actual backends that get linked and used, e.g. emberHeadlessApp uses all the nullX backends as it doesnt need a window or gui.
Each application type comes with its own additional code. E.g. the editorApp contains the editorWindows.
To create a new project simply link any of these applications into your project and you are good to go.
Examples of this can be found in the projects folder (Ember/projects/<project name>).
The other folders in the Ember/engine folder are:
Ember/engine/common     ->   header only communication structs/enums which get included by core and backends.
Ember/engine/extern     ->   external libraries (submodules).
Ember/engine/interfaces ->   interfaces which get implemented by the backends.
Ember/engine/libs       ->   static libraries without global/static state that can be used by core and backends, e.g. assetLoader, entityComponentSystem, logger, math, profiler, spirvReflect, threadPool.
Ember/engine/shaders    ->   basic shaders which are shipped with the engine by default; more can be added in your own project.



Unit Tests:
Each library and backend contains its own unit test project (when applicable, not everything has unit tests) inside its folder, e.g. Ember/engine/backends/vulkanRenderer/unitTests/CMakeLists.txt.
These unit tests can either be build indepedently, or all be build+run at the same time via the meta unit test proect at Ember/unitTests/CMakeLists.txt, simply execute the runAllTests.bat/.sh script.
The project at Ember/projects/unitTests/CMakeLists.txt, contains higher level integration tests, e.g. compute shaders, which are easier/better to test in the context of the entire engine.



Project Setup:
To create a new project simply link the desired base application from 'Ember/engine/application/*' in your CMakeLists.txt via:
    add_subdirectory("../../engine/applications/emberEditorApp" EmberEditorApp)
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberEditorApp)



How to compile (Windows):
Start CMake and use the following configuration:
Where is the source code:    <parent folder of CMakeLists.txt, e.g. C:/.../Ember/project/defaultProject>
Where to build the binaries: <path to build folder,            e.g. C:/.../Ember/project/defaultProject/build>
Press the buttons 'configure' and then 'generate'.

You should now have a Visual Studio project 'defaultProject.sln' in the build folder.
Open the .sln with Visual Studio 2022 or later, right-click the 'DefaultProject' solution, and select 'Set as Startup Project'.
You can now build and run the code with the play button or 'ctrl + F5'.



How to compile (Linux):
In a terminal navigate to the project you want to compile, e.g. 'home/<username>/Documents/Ember/projects/defaultProject'.
Create a build folder:      mkdir build
navigate into it:           cd build
run cmake:                  cmake ..
run make:                   make (optional: make -j, for multi core compilation)
navigate to bin:            cd ../bin/<buildType>
execute the project:        ./<Project Name>
Alternatively you can setup visual studio or any IDE of your liking to compile/run the cmake project.



Implemented features:
- Modular architecture with a backend-independent core and interfaces, plus separate Vulkan renderer, SDL window, ImGui, and null backends.
- Reusable libraries for asset loading, buffer layouts, data structures, ECS, editor tools, logging, math, shader reflection, task scheduling/profiling, and Vulkan utilities.
- Editor functionality is isolated from the core and linked only by applications that need it.
- Preconfigured application modes:
    - emberGameApp:      sdlWindow, nullGui.
    - emberGameDebugApp: sdlWindow, imGuiSdlVulkan.
    - emberEditorApp:    sdlWindow, imGuiSdlVulkan + custom editor window classes.
    - emberHeadlessApp:  nullWindow, nullGui.
- Vulkan render graph with explicit synchronization across graphics and compute queues:
 Acquire
 └>ResourceUpdate
   ├> Gizmo ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┬> Present ─> Release
   └> PreRenderCompute ┬> Outline ──────────> MidRenderCompute  ────────────────────────────────────────────────────────────┬> PostRenderCompute ┘
                       ├> Shadow ──────────┬> DeferredLighting ─> ForwardOpaque ─> ScreenSpaceCompute ─> ForwardTransparent ┘
                       └> DeferredGeometry ┘
- Rendering features:
    - Deferred geometry and lighting, forward opaque and transparent rendering, shadow maps, object outlines, gizmos, and presentation/post-processing passes.
    - Directional, point, and spot lights with configurable light limits and shadow-map resolution.
    - Configurable VSync, frames in flight, render resolution, and MSAA sample count.
    - Interleaved and separate vertex-buffer layouts, indexed meshes, instanced rendering, and transparent draw sorting.
- Compute features:
    - Pre-render, mid-render, screen-space, and post-render compute queues integrated into the render graph.
    - Asynchronous compute sessions with explicit barriers, completion polling, and a triple-buffered physics workflow.
    - GPU bitonic sorting with optional permutation generation and application.
- GPU resources and asset management:
    - Managed vertex, index, uniform, storage, and staging buffers.
    - Sampled, storage, render-target, depth, G-buffer, and combined sampled/storage textures in 1D, 2D, 3D, array, and cube variants where applicable.
    - Dynamic, multi-buffered meshes and triple-buffered data for CPU/GPU update workflows.
    - Named resource managers for buffers, textures, meshes, materials, material shaders, and compute shaders.
    - Image and cubemap loading, CSV mesh loading, and JSON material+computeShader assets.
    - Batched texture uploads, staging-buffer reuse, descriptor-pool management, VMA-backed allocation, allocation tracking, and deferred Vulkan object destruction.
- Descriptor and shader system:
    - Five descriptor sets ordered by update frequency: global (set 0), scene (set 1), frame (set 2), shader/material (set 3), and draw/dispatch call (set 4).
    - Static global, scene, and frame sets are managed by the renderer; dynamic shader and call sets are generated from SPIR-V reflection.
    - Reflected descriptor layouts and automatic descriptor binding for materials, compute shaders, and per-call properties.
    - HLSL shader compilation for vertex, fragment, and compute stages, with shared C++/HLSL constants and structures.
- Scene and gameplay systems:
    - Entity-component system with scenes, entities, component registration, lifecycle callbacks, transforms, cameras, lights, mesh renderers, instanced renderers, and post-render effects.
    - SDL event routing with separate game/scene, GUI, and editor consumers.
    - Fixed-step physics update scheduling and GPU-compute integration.
    - Editor-style camera controller.
- Editor and GUI:
    - Dear ImGui integration with docking and detached editor-window support.
    - Scene selection, hierarchy and inspector tooling, configurable outlines, and translate/rotate/scale/bounds manipulation handles.
    - Game, scene, project, console, FPS, depth-bias, and Vulkan backend debug windows.
- Foundation libraries and diagnostics:
    - Custom vector, matrix, random, interpolation, sorting, bounds, ray, plane, triangle, sphere, capsule, cone, quad, and intersection math.
    - Parallel thread pool, hierarchical CPU profiler, structured logging, reusable index allocation, and reflected buffer-layout utilities.
    - Vulkan validation-layer support, debug object naming, and human-readable Vulkan/VMA diagnostics.





Useful Information:
Device Info: https://vulkan.gpuinfo.org/
Vulkan documentation: https://vulkan.lunarg.com/doc/sdk
HLSL alignment visualizer: https://maraneshi.github.io/HLSL-ConstantBufferLayoutVisualizer/