Prerequisites:
To compile this code, you need to have Vulkan SDK 1.3.290.0 (or later) and cmake version 3.30 (or later) installed.
All other dependencies (googletest, imgui, SDL, spdlog, SPIRV-Reflect, vma) are submodules and will be compiled from source.
(Don't forget to clone recursively for the submodules: git clone --recurse-submodules <repository-url>, or after cloning do: git submodule update --init --recursive)



Architecture:
The engine consists of a core (Ember/engine/core) and multiple backend libraries: imGuiSdlVulkan, nullGui, nullWindow, sdlWindow, vulkanRenderer (Ember/engine/backends).
The core and the backends all build into shared libraries which then get linked into multiple statically build applications (Ember/engine/applications), emberEditorApp, emberGameApp, emberGameDebugApp, emberHeadlessApp.
The difference between these applications is the actual backends that get linked and used, e.g. emberHeadlessApp uses all the nullX backends as it doesnt need a window or gui.
Each application type comes with its own additional code. E.g. the editorApp contains the editorWindows.
To create a new project simply link any of these applications into your project and you are good to go.
Examples of this can be found in Ember/projects/... .
The other folders in the Ember/engine folder are:
Ember/engine/common     ->   header only communication structs/enums which get included by core and backends.
Ember/engine/extern     ->   external libraries (submodules).
Ember/engine/interfaces ->   interfaces which get implemented by the backends.
Ember/engine/libs       ->   static libraries without global/static state that can be used by core and backends, e.g. assetLoader, logger, math, profiler, threadPool.
Ember/engine/shaders    ->   basic shaders which are shipped with the engine by default; more can be added in your own project.



Project Setup:
To create a new project simply link the desired base application from 'Ember/engine/application/*' in your CMakeLists.txt via:
    add_subdirectory("../../engine/applications/emberEditorApp" EmberEditorApp)
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
-clean separation of editor code into its own application.
-Pipelines:
    -preRender compute (instancing data manipulation).
    -shadoow mapping (multiple light sources, directional/positional lights, physical based(roughnessMap, normalMap, metallicity, reflectivity), shadow cascades with shadow snapping, instanced rendering support).
    -forward renderpass (opaque + skybox + transparent, instanced rendering support).
    -postRender compute (post processing effects).
    -async comppute.
    -immediate compue.
-Fully automated descriptorSet system for shaderProperties (see spirvReflect.h/cpp) which handles descriptorSets for Materials and ComputeShaders.
-ECS: static library in the Embere/engine/libs folder which gets linked to the applications. The core has no access to the ECS.
-EventSystem that catches SDL events and makes them visible to Gui+Editor+Entities/Components.
-CameraController that is identical to unities editor.
-Custom math library, see math.h/cpp. (will be refactored to be a glm wrapper soon).
-Dear ImGui integration with docking feature.



Ember::TODO now!
- add BufferView<T> with T=all math types which allows for stricter compile time buffer property restrictions.
- performance worse than before architecture restructure.
- make other projects compile and run again. => currently working on this! unitTest works, fluidDynamics is on its way. headless mode needs testing for proper unitTests.
- math::Random has static state and is in a static library that gets linked multiple times => inconcistent state. Fix this.
- rewrite math library to be a wrapper around glm?
- Add proper const to all methods that return a member pointer:
  const Type* const Foo()...:
  fist const: interpreted as (const Type*) meaning the value cant be changed.
  second const: the adress inside the pointer cant be chaged.
- target_link_libraries(backends) with PRIVATE keyword?
- better unified model for descriptorset bindings e.g: 0-127 for textures, 128-255 for buffers, etc.
- descriptorsetpool management with dynamic creation of further pools when needed.
- use one and he same descriptorset for all shadowproperties? meaning i would have to bind multiple shaderproperties to each draw call.
- improve PercentageCloserFilteredShadow (shadowMapping.hlsli) to work across shadowmap boundaries.
- sort entities first by material (to reduce pipeline changes) and then by proximity to pCamera to reduce fragment culling (render closer objects first)
- implement frustum culling.
- validation layer errors when two shaders have the same binding number (binding missmatch error)
- add SetData(...) to emberBackendInterface::IBuffer interface and appropiate overloads to vertex/index/storage buffers.
  Also add void* data to CreateBuffer factory.
- vmaBuffer and Buffer have a m_name member, same for vmaImage and Texture. One should be enough, having both is redundant?
- move emberCommon::... to backend::... conversion to where its needed when its only ever needed once.
- headless mode



Ember::TODO:
- use transfer queue in mesh class instead of graphics queue for index and vertex data transfer.
  use framesInFlight many vertex and index buffers, with two staging buffers (one for vertex and one for index data).
  adjust staging buffer -> mesh buffers copy to account fro frame index (same as shaderProperties) and sync with render pipeline.
- when not using input.vertexColor in the vertex shader, spirv optimizes the input binding away, which leads to incorrect bindings in
  my spirv reflection => other bindings are wrong, and textures are not displayed at all.
- optimize eventsystem::AnyKey etc.
- remove scaling from view matrizes of lights and cameras?
- optimizations: multi threaded render loop, culling, etc.
- add entity selection (need gizmos => ui renderpass?)
- uniform buffer (cbuffer) data that is the same for all draw calls (e.g. light data) should be stored in a single cbuffer
  that is bound to a single descriptorSet that is bound to all draw calls that need that data.
  => make this descriptorset a "global" object in the shaderProperties class.
- add geometry shader stage => wireframe rendering
- entity parent system (entity € entity => transform hierarchy)
- add logic to mesh class to only update the parts of the buffer that have changed (e.g. pos, normal, ...)
- add debugOnly assert to check if 'normal' vectors are normalized and 'tangent' vectors are orthogonal to 'normal' vectors.
  remove normalization of any input vector that is namen 'normal' or 'tangent' or 'direction' in mathf library (same for linearAlgebra.hlsli).

Ember::TODO long term:
- dimm line between shadow cascades.
- change image loading library, stb_image sucks.
- proper quaternion support
- render image while resizing
- physics
- audio
- entity clipping logic for pCamera and lights (requires bounding box)
- better shadow mapping (PCF, soft shadows, etc.)
- blender model import
- text rendering
- only load textures that are used. parallel loading of textures.



Useful Information:
Device Info: https://vulkan.gpuinfo.org/
Vulkan documentation: https://vulkan.lunarg.com/doc/sdk