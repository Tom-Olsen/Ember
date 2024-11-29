#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"



// TODO now:
// - refactor code with proper coding style!
// - directional lights: shadow cascades
// - validation layer errors when two shaders have the same binding number
// - add gameObject selection (need gizmos => ui renderpass)
// - currently one commandPool per commandBuffer, should be one commandPool per frame, shared by all commands for that frame
// - uniform buffer (cbuffer) data that is the same for all draw calls (e.g. light data) should be stored in a single cbuffer
//   that is bound to a single descriptorSet that is bound to all draw calls that need that data.
//   => make this descriptorset a "global" object in the materialProperties class.
// - shadowMapping.hlsli: PhysicalDirectionalLights(...) has depth bias added manually. Should be done via pipeline rasterization state.

// TODO long term:
// - change image loading library, stb_image sucks.
// - proper quaternion support
// - ui renderpass that draws on top of everything and is not affected by the camera (constant view/projection matrix)
// - render image while resizing
// - implement game physics fixedUpdate loop
// - post processing
// - particles (instancing)
// - physics
// - audio
// - gameobject clipping logic for camera and lights (requires bounding box)
// - change shared ptr in VulkanUniformBuffer.buffer to unique ptr
// - support arrays in spirv reflection
// - write own logger class
// - better shadow mapping (PCF, soft shadows, etc.)
// - engine name?
// - compile engine as .dll and link to game/test project
// - imgui integration
// - render into custom image with higher precision (16-bit floats for color instead of 8-bit) and push final image to swapchain.
//   This allows for HDR rendering and post processing effects.
//    - look into colorspace sRGB vs linear.
// - blender model import
// - text rendering



int main()
{
    // VS debugging:
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Initialization:
    Logger::Init();
    Application app;

    //LOG_TRACE(material->GetUniformBufferBlock("LightMatrizes")->ToString());
    //MaterialManager::GetMaterial("defaultMaterial")->PrintBindings();
    //MaterialManager::GetMaterial("skyboxMaterial")->PrintBindings();
    //MaterialManager::GetMaterial("colorMaterial")->PrintUniformBuffers();
    //MaterialManager::GetMaterial("defaultMaterial")->PrintUniformBuffers();
    // return 0;

    bool directionalLightsActive = 0;
    bool spotLightsActive = !directionalLightsActive;

    // Build simple scene:
    Scene* scene = new Scene();
    {// Camera:
        GameObject* gameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, 3.0f, 5.0f);
        gameObject->transform->SetPosition(pos);
        gameObject->transform->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up));
    
        Camera* camera = new Camera();
        gameObject->AddComponent<Camera>(camera);
    
		CameraController* cameraController = new CameraController();
        gameObject->AddComponent<CameraController>(cameraController);
    
        scene->AddGameObject(gameObject);
		scene->SetActiveCamera(camera);
    }
    {// PointLight:
        GameObject* gameObject = new GameObject("pointLight");
        Float3 pos = Float3(1.0f, 2.0f, 0.0f);
        Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
        gameObject->transform->SetPosition(pos);
        gameObject->transform->SetRotationMatrix(matrix);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("colorMaterial"));
        meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTexture2d("white"));
        meshRenderer->castShadows = meshRenderer->receiveShadows = false;
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        PointLight* pointLight = new PointLight();
        pointLight->SetIntensity(10.0f);
		pointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pointLight->SetNearClip(0.1f);
		pointLight->SetFarClip(20.0f);
		gameObject->AddComponent<PointLight>(pointLight);
    
        SpinGlobal* spinGlobal = new SpinGlobal(Float3::zero, Float3(0, 45, 0));
        gameObject->AddComponent<SpinGlobal>(spinGlobal);
    
        scene->AddGameObject(gameObject);
    }
    {// Light0:
        GameObject* gameObject = new GameObject("light0");
        Float3 pos = Float3(7.0f, 7.0f, 3.5f);
        Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
        gameObject->transform->SetPosition(pos);
        gameObject->transform->SetRotationMatrix(matrix);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("colorMaterial"));
        meshRenderer->castShadows = meshRenderer->receiveShadows = false;
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        //SpinGlobal* spinGlobal = new SpinGlobal(Float3::zero, Float3(0, 45, 0));
        //gameObject->AddComponent<SpinGlobal>(spinGlobal);
    
        if (directionalLightsActive)
        {
            DirectionalLight* directionalLight = new DirectionalLight();
            directionalLight->SetIntensity(1.0f);
            directionalLight->SetColor(Float3(1.0f, 0.0f, 0.0f));
            directionalLight->SetNearClip(0.1f);
            directionalLight->SetFarClip(15.0f);
            directionalLight->SetViewWidth(10.0f);
            directionalLight->SetViewHeight(10.0f);
            gameObject->AddComponent<DirectionalLight>(directionalLight);
        }
    
        if (spotLightsActive)
        {
            SpotLight* spotLight = new SpotLight();
            spotLight->SetColor(Float3(1.0f, 0.0f, 0.0f));
            spotLight->SetIntensity(100.0f);
            spotLight->SetNearClip(0.1f);
            spotLight->SetFarClip(20.0f);
            spotLight->SetFovDegrees(30.0f);
            spotLight->SetBlendStart(0.7f);
            spotLight->SetBlendEnd(0.9f);
            gameObject->AddComponent<SpotLight>(spotLight);
        }
    
		scene->AddGameObject(gameObject);
    }
    {// Light1:
        GameObject* gameObject = new GameObject("light1");
        Float3 pos = Float3(-7.0f, 7.0f, 3.5f);
        Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
        gameObject->transform->SetPosition(pos);
        gameObject->transform->SetRotationMatrix(matrix);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("colorMaterial"));
        meshRenderer->castShadows = meshRenderer->receiveShadows = false;
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        //SpinGlobal* spinGlobal = new SpinGlobal(Float3::zero, Float3(0, -60, 0));
        //gameObject->AddComponent<SpinGlobal>(spinGlobal);
    
        if (directionalLightsActive)
        {
            DirectionalLight* directionalLight = new DirectionalLight();
            directionalLight->SetIntensity(1.0f);
            directionalLight->SetColor(Float3(0.0f, 1.0f, 0.0f));
            directionalLight->SetNearClip(0.1f);
            directionalLight->SetFarClip(15.0f);
            directionalLight->SetViewWidth(10.0f);
            directionalLight->SetViewHeight(10.0f);
            gameObject->AddComponent<DirectionalLight>(directionalLight);
        }
    
        if (spotLightsActive)
        {
            SpotLight* spotLight = new SpotLight();
            spotLight->SetColor(Float3(0.0f, 1.0f, 0.0f));
            spotLight->SetIntensity(100.0f);
            spotLight->SetNearClip(0.1f);
            spotLight->SetFarClip(20.0f);
            spotLight->SetFovDegrees(30.0f);
            spotLight->SetBlendStart(0.7f);
            spotLight->SetBlendEnd(0.9f);
            gameObject->AddComponent<SpotLight>(spotLight);
        }
    
        scene->AddGameObject(gameObject);
    }
    {// Light2:
        GameObject* gameObject = new GameObject("light2");
        Float3 pos = Float3(0.0f, 7.0f, -7.5f);
        Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
        gameObject->transform->SetPosition(pos);
        gameObject->transform->SetRotationMatrix(matrix);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("colorMaterial"));
        meshRenderer->castShadows = meshRenderer->receiveShadows = false;
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        //SpinGlobal* spinGlobal = new SpinGlobal(Float3::zero, Float3(0, 90, 0));
        //gameObject->AddComponent<SpinGlobal>(spinGlobal);
    
        if (directionalLightsActive)
        {
            DirectionalLight* directionalLight = new DirectionalLight();
            directionalLight->SetIntensity(1.0f);
            directionalLight->SetColor(Float3(0.0f, 0.0f, 1.0f));
            directionalLight->SetNearClip(0.1f);
            directionalLight->SetFarClip(15.0f);
            directionalLight->SetViewWidth(10.0f);
            directionalLight->SetViewHeight(10.0f);
            gameObject->AddComponent<DirectionalLight>(directionalLight);
        }
    
        if (spotLightsActive)
        {
            SpotLight* spotLight = new SpotLight();
            spotLight->SetColor(Float3(0.0f, 0.0f, 1.0f));
            spotLight->SetIntensity(100.0f);
            spotLight->SetNearClip(0.1f);
            spotLight->SetFarClip(20.0f);
            spotLight->SetFovDegrees(30.0f);
            spotLight->SetBlendStart(0.7f);
            spotLight->SetBlendEnd(0.9f);
            gameObject->AddComponent<SpotLight>(spotLight);
        }
    
        scene->AddGameObject(gameObject);
    }
    {// Skybox:
        GameObject* gameObject = new GameObject("skybox");
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
        meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTextureCube("skyboxClouds0"));
        meshRenderer->receiveShadows = meshRenderer->castShadows = false;
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
    }
    { // Floor:
        GameObject* gameObject = new GameObject("floor");
        gameObject->transform->SetPosition(0.0f, -0.5f, 0.0f);
        gameObject->transform->SetScale(30.0f);
        gameObject->transform->SetRotationEulerDegrees(-90.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitQuad");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTexture2d("white"));
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
    }
    { // Wall:
        GameObject* gameObject = new GameObject("wall");
        gameObject->transform->SetPosition(0.0f, 2.5f, -10.0f);
        gameObject->transform->SetScale(Float3(30.0f, 6.0f, 1.0f));

        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitQuad");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTexture2d("brick"));
        meshRenderer->materialProperties->SetValue("SurfaceProperties", "roughness", 1.0f);
        meshRenderer->materialProperties->SetValue("SurfaceProperties", "scaleOffset", Float4(10, 2, 1, 1));
        gameObject->AddComponent<MeshRenderer>(meshRenderer);

        scene->AddGameObject(gameObject);
    }
    {// ThreeLeg:
        GameObject* gameObject = new GameObject("threeLeg");
        gameObject->transform->SetPosition(-2.0f, 0.0f, 1.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("colorMaterial"));
        //meshRenderer->castShadows = meshRenderer->receiveShadows = false;
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
    }
    {// Cube0:
        GameObject* gameObject = new GameObject("cube0");
        gameObject->transform->SetPosition(-1.0f, 0.0f, -1.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTexture2d("brick"));
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        SpinLocal* spinLocal = new SpinLocal(Float3(0.0f, 45.0f, 0.0f));
        gameObject->AddComponent<SpinLocal>(spinLocal);
    
        scene->AddGameObject(gameObject);
    }
    {// Cube1:
        GameObject* gameObject = new GameObject("cube1");
        gameObject->transform->SetPosition(2.0f, 0.0f, 0.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTexture2d("stones"));
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
    }
    {// Sphere 0:
        GameObject* gameObject = new GameObject("sphere0");
        gameObject->transform->SetPosition(0.5f, 0.0f, 0.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("cubeSphere");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTexture2d("wall0"));
        meshRenderer->materialProperties->SetValue("SurfaceProperties", "roughness", 1.0f);
        gameObject->AddComponent<MeshRenderer>(meshRenderer);

        //SpinGlobal* spinGlobal = new SpinGlobal(Float3(2.0f, 0.0f, 0.0f), Float3(0, 45, 0));
        //gameObject->AddComponent<SpinGlobal>(spinGlobal);
    
        scene->AddGameObject(gameObject);
    }
    {// Sphere 1:
        GameObject* gameObject = new GameObject("sphere1");
        gameObject->transform->SetPosition(-0.5f, 0.0f, 1.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("cubeSphere");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTexture2d("wood0"));
        meshRenderer->materialProperties->SetValue("SurfaceProperties", "roughness", 0.5f);
        gameObject->AddComponent<MeshRenderer>(meshRenderer);

        SpinLocal* spinLocal = new SpinLocal(Float3(0.0f, 30.0f, 0.0f));
        gameObject->AddComponent<SpinLocal>(spinLocal);

        scene->AddGameObject(gameObject);
    }
    {// Cube Array:
        int N = 3;
        float dist = 4.0f;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                GameObject* gameObject = new GameObject("cube" + std::to_string(i) + std::to_string(j));
				Float3 pos((i / (N - 1.0f) - 0.5f) * dist, (j / (N - 1.0f) - 0.5f) * dist + 0.5f * dist, -8.0f);
                gameObject->transform->SetPosition(pos);
                gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
                MeshRenderer* meshRenderer = new MeshRenderer();
                meshRenderer->mesh = MeshManager::GetMesh("unitCube");
                meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
                meshRenderer->materialProperties->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
                meshRenderer->materialProperties->SetTexture2d("colorTexture", TextureManager::GetTexture2d("stones"));
                meshRenderer->materialProperties->SetValue("SurfaceProperties", "roughness", 0.8f);
                gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
                scene->AddGameObject(gameObject);
            }
    }
    //scene->PrintGameObjects();
    //scene->PrintMeshRenderers();
    //scene->PrintSortedMeshRenderers();
    //scene->PrintLights();
    app.SetScene(scene);



    // Run application:
    try
    {
        app.Run();
    }
    catch (const std::exception& e)
    {
		LOG_ERROR("Exception: {}", e.what());
    }



    // Terminate:
    delete scene;
    return 0;
}