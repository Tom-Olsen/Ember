Prerequisites:
To compile this code, you need to have Vulkan SDK 1.3.290.0 (or later) and cmake version 3.30 (or later) installed.
All other dependencies (googletest, imgui, SDL, spdlog, SPIRV-Reflect, vma) are submodules and will be compiled from source.
(Don't forget to clone recursively for the submodules: git clone --recurse-submodules <repository-url>)



Architecture:
The engine consists of a core (Ember/engine/core) and multiple backend libraries: emberEditor, imGuiSdlVulkan, nullEditor, nullGui, nullWindow, sdlWindow, vulkanRenderer (Ember/engine/backends).
The core and the backends all build into shared libraries which then get linked into multiple statically build applications (Ember/applications), emberEditorApp, emberGameApp, emberGameDebugApp, emberHeadlessApp.
The difference between these applications is the actual backends that get linked and used, e.g. emberHeadlessApp uses all the nullX backends as it doesnt need a window, gui or an editor.
To create a new project simply link any of these applications into your project and you are good to go.
Examples of this can be found in Ember/projects/... .
The other folders in the Ember/engine folder are:
Ember/engine/common     ->   header only communication structs/enums which get included by core and backends.
Ember/engine/extern     ->   external libraries (submodules).
Ember/engine/interfaces ->   interfaces which get implemented by the backends.
Ember/engine/libs       ->   static libraries without global/static state that can be used by core and backends, e.g. assetLoader, logger, math, profiler, threadPool.
Ember/engine/shaders    ->   basic shaders which are shipped with the engine by default; more can be added in your own project.



Project Setup:
To create a new project simply link the desired base application from 'Ember/application/*' in your CMakeLists.txt via:
    add_subdirectory("../../applications/emberEditorApp" EmberEditorApp)
    target_link_libraries(${PROJECT_NAME} PUBLIC EmberEditorApp)



How to compile (Windows 10):
Start CMake and use the following configuration:
Where is the source code:    <parent folder of CMakeLists.txt, e.g. C:/.../Ember/project/defaultProject>
Where to build the binaries: <path to build folder,            e.g. C:/.../Ember/project/defaultProject/build>
Press the buttons 'configure' and then 'generate'.

You should now have a Visual Studio project 'defaultProject.sln' in the build folder.
Open the .sln with Visual Studio 2022 or later, right-click the 'DefaultProject' solution, and select 'Set as Startup Project'.
You can now build and run the code with the play button or 'ctrl + F5'.



How to compile (Ubuntu):
In a terminal navigate to the project you want to compile, e.g. 'home/<username>/Documents/Ember/projects/defaultProject'.
Create a build folder:      mkdir build
navigate into the it:       cd build
run cmake:                  cmake ..
run make:                   make (optional: make -j, for multi core compilation)
navigate to bin:            cd ../bin/<buildType>
execute the project:        ./<Project Name>
Alternatively you can setup visual studio or any IDE of your liking to compile/run the cmake project.



Implemented features:
-clean architecture split of core and backends
-multiple application modes:
    -emberGameApp:         sdlWindow, nullGui.
    -emberGameDebugApp:    sdlWindow, imGuiSdlVulkan.
    -emberEditorApp:       sdlWindow, imGuiSdlVulkan + custom editor window classes.
    -emberHeadlessApp:     nullWindow, nullGui.
-clean separation of editor into its own applicationl.
-Pipelines:
    -preRender compute (instancing data manipulation).
    -shadoow mapping (multiple light sources, directional/positional lights, physical based(roughnessMap, normalMap, metallicity, reflectivity), shadow cascades with shadow snapping, instanced rendering support).
    -forward renderpass (opaque + skybox + transparent, instanced rendering support).
    -postRender compute (post processing effects).
    -async comppute.
    -immediate compue.
-Fully automated descriptorSet system for shaderProperties (see spirvReflect.h/cpp) which handles descriptorSets for Materials and ComputeShaders.
-ECS.
-EventSystem that catches SDL events and makes them visible to Gui+Editor+GameObjects/Components.
-CameraController that is identical to unities editor pCamera.
-Custom math library, see math.h/cpp. (will be refactored to be a glm wrapper soon).
-Dear ImGui integration with docking feature.



Ember::TODO now!
- crash on shutdown: 5 vma allocations are dangling.
- performance worse than before architecture restructure.
- make other projects compile and run again.
- build core as .dll
- target_link_libraries(backends) with PRIVATE keyword?
- gpu resources on core side should always be handled as value/reference types as they are simple wrappers around a single pointer.
- go through all classes and implement rule of 5 properly:
  If any of the following is defined, define all of them!
  1. Destructor					~Foo();
	 2. Copy constructor			Foo(const Foo& other);
  3. Copy assignment operator	Foo& operator=(const Foo& other);
  4. Move constructor			Foo(Foo&& other);
	 5. Move assigment operator		Foo& operator=(Foo&& other);
- improve PercentageCloserFilteredShadow (shadowMapping.hlsli) to work across shadowmap boundaries.
- sort gameObjects first by material (to reduce pipeline changes) and then by proximity to pCamera to reduce fragment culling (render closer objects first)
- validation layer errors when two shaders have the same binding number (binding missmatch error)
- SceneView gui 
- add SetData(...) to emberBackendInterface::IBuffer interface and appropiate overloads to vertex/index/storage buffers.
  Also add void* data to CreateBuffer factory.
- add name to vulkanRendererBackend::Buffer? (also add Get/SetName(...) in IBuffer).
- move emberCommon::... to backend::... conversion to where its needed when its only ever needed once.



// Ember::TODO:
// - use transfer queue in mesh class instead of graphics queue for index and vertex data transfer.
//   use framesInFlight many vertex and index buffers, with two staging buffers (one for vertex and one for index data).
//   adjust staging buffer -> mesh buffers copy to account fro frame index (same as shaderProperties) and sync with render pipeline.
// - think about different texture2d specializations. Are they all needed? Can they be simplified? How would I remove the m_layout tracking on CPU side?
// - when not using input.vertexColor in the vertex shader, spirv optimizes the input binding away, which leads to incorrect bindings in
//   my spirv reflection => other bindings are wrong, and textures are not displayed at all.
// - optimize eventsystem::AnyKey etc.
// - remove scaling from view matrizes of lights and cameras?
// - adjust shadow config in shadowPipeline.cpp (bias values) for better shadow quality
// - optimizations: multi threaded render loop, culling, etc.
// - add pGameObject selection (need gizmos => ui renderpass?)
// - uniform buffer (cbuffer) data that is the same for all draw calls (e.g. light data) should be stored in a single cbuffer
//   that is bound to a single descriptorSet that is bound to all draw calls that need that data.
//   => make this descriptorset a "global" object in the shaderProperties class.
// - add geometry shader stage => wireframe rendering
// - gameobject parent system (GameObject € GameObject => transform hierarchy)
// - add logic to mesh class to only update the parts of the buffer that have changed (e.g. pos, normal, ...)
// - add debugOnly assert to check if 'normal' vectors are normalized and 'tangent' vectors are orthogonal to 'normal' vectors.
//   remove normalization of any input vector that is namen 'normal' or 'tangent' or 'direction' in mathf library (same for linearAlgebra.hlsli).
// - Proper documentation for shaderProperties, uniformBuffers, spirvReflect classes

// Ember::TODO long term:
// - dimm line between shadow cascades.
// - change image loading library, stb_image sucks.
// - proper quaternion support
// - render image while resizing
// - physics
// - audio
// - gameobject clipping logic for pCamera and lights (requires bounding box)
// - write own logger class
// - better shadow mapping (PCF, soft shadows, etc.)
// - look into colorspace sRGB vs linear.
// - blender model import
// - text rendering
// - only load textures that are used. parallel loading of textures.



Useful Information:
Device Info: https://vulkan.gpuinfo.org/
Vulkan documentation: https://vulkan.lunarg.com/doc/sdk
