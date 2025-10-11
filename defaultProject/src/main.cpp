#define SDL_MAIN_HANDLED
#include "profiler.h"
#include "gameApplication.h"
#include "emberEngine.h"
// Scenes:
#include "defaultScene.h"
#include "pointLightScene.h"
#include "shadowCascadeScene.h"
#include "singleQuadScene.h"
#include "testScene.h"
using namespace emberEngine;

// Ember::TODO now!
// - gpu resources on core side should always be handled as value/reference types as they are simple wrappers around a single pointer.
// - go through all classes and implement rule of 5 properly:
//   If any of the following is defined, define all of them!
//   1. Destructor					~Foo();
//	 2. Copy constructor			Foo(const Foo& other);
//   3. Copy assignment operator	Foo& operator=(const Foo& other);
//   4. Move constructor			Foo(Foo&& other);
//	 5. Move assigment operator		Foo& operator=(Foo&& other);
// - improve PercentageCloserFilteredShadow (shadowMapping.hlsli) to work across shadowmap boundaries.
// - sort gameObjects first by material (to reduce pipeline changes) and then by proximity to pCamera to reduce fragment culling (render closer objects first)
// - validation layer errors when two shaders have the same binding number (binding missmatch error)
// - SceneView gui 

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

// Implemented Features:
// - pre render compute shaders.
// - shadow pipeline
// - forward render pipeline.
// - post render compute shaders.
// - async & immediate compute.
// - Multi lightsoure shadow mapping (directionallights, positionallights).
// - Physical based lighting (roughnessMap, normalMap, metallicity, reflectivity).
// - Shadow cascades with shadow snapping.
// - CubeMap (TextureCube) skybox.
// - Automated descriptorSet system for shaderProperties (see spirvReflect.h/cpp) which handles descriptorSets for Materials and ComputeShaders.
// - Component € GameObject € Scene system with game update loop.
// - Graphics::Draw(...) allows for drawCall injection into render pipeline from GameObject Components.
// - EventSystem that catches SDL events and makes them visible to all GameObjects/Components.
// - CameraController that is identical to unities editor pCamera.
// - Own math library, see math.h/cpp.
// - Dear ImGui integration with docking feature.
// - Batched texture uploading.



int main()
{
	// VS debugging (windows only=MSVC compiler):
	#ifdef _MSC_VER
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	// Profiler:
	EmberProfiler::Session& session = EmberProfiler::Session::Get();
	session.Start("profiling", "profilingResults");
	{
		// Initialization:
		Application::CreateInfo appCreateInfo = {};
		appCreateInfo.vSyncEnabled = false;
		appCreateInfo.framesInFlight = 2;
		appCreateInfo.msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount04;
		appCreateInfo.windowWidth = 2560;//1600; //1920; // 2560;
		appCreateInfo.windowHeight = 1440;// 900; //1080; // 1440;
		appCreateInfo.renderWidth = 2560;//1280; //1280; // 2560;
		appCreateInfo.renderHeight = 1440;// 720; // 720; // 1440;
		Application app(appCreateInfo);

		// Create scene:
		// std::unique_ptr<Scene> pScene = std::unique_ptr<Scene>(ShadowCascadeScene());
		// std::unique_ptr<Scene> pScene = std::unique_ptr<Scene>(TestScene());
		std::unique_ptr<Scene> pScene = std::unique_ptr<Scene>(DefaultScene());
		// std::unique_ptr<Scene> pScene = std::unique_ptr<Scene>(PointLightScene());
		// std::unique_ptr<Scene> pScene = std::unique_ptr<Scene>(SingleQuadScene());
		app.SetScene(pScene.get());

		// Debugging:
		//ComputeShader* csTest =  ComputeShaderManager::GetComputeShader("testComputeShader");
		//csTest->PrintShaderInfo();
		//ShaderProperties* csProperties = new ShaderProperties(csTest);
		//csProperties->Print("test");
		//csProperties->PrintMaps();
		//csProperties->SetTexture("image", TextureManager::GetTexture("storageTexture8x8"));
		//delete csProperties;
		//TextureManager::PrintAllTextureNames();
		//pScene->PrintGameObjects();
		//pScene->PrintLights();
		//return 0;

		app.Run();
	}

	// Runtime analysis:
	EmberProfiler::Session::Get().End();
	std::vector<std::string> results = session.GetAllResultNames();
	for (std::string& result : results)
		session.PrintFunctionAverageTime(result, EmberProfiler::TimeUnit::ms);

	return 0;
}