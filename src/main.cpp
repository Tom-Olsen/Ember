#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"



// TODO:
// - proper quaternion support
// - reduce normalMatrix to 3x3 matrix in transform and shader
// - 3d ui renderpass that draws on top of everything and is not affected by the camera (constant view/projection matrix)
// - own math lybrary (arrow rotation not working properly)
// - input system (get inputs from SdlWindow->HandlEvents() and send them to the active scene)
// - implement camera projection matrix by my own
// - render image while resizing
// - implement game physics fixedUpdate loop
// - camera controller script
// - shadows
// - post processing
// - skybox
// - particles (instancing)
// - physics
// - audio
// - Material parent class with inheritance: ForwardMaterial, ShadowMaterial
// - gameobject clipping logic (requires bounding box)



int main()
{
    // VS debugging:
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Initialization:
    Logger::Init();
    Application app;

    // Build simple scene:
    Scene* scene = new Scene();
    {// Camera:
        GameObject* camera = new GameObject("mainCamera");
        camera->transform->SetPosition(Float3(0.0f, -5.0f, 5.0f));
        camera->transform->SetEulerDegrees(Float3(-45.0f, 0.0f, 0.0f));

        Camera* cameraComponent = new Camera();
        camera->AddComponent<Camera>(cameraComponent);

        //Spin* spin = new Spin(Float3(20.0f, 0.0f, 0.0f));
        //spin->logAngles = true;
        //camera->AddComponent<Spin>(spin);

        scene->AddGameObject(camera);
		scene->SetActiveCamera(cameraComponent);
    }
    {// Sun:
        GameObject* sun = new GameObject("sun");
        sun->transform->SetEulerDegrees(Float3(-45.0f, -135.0f, 0.0f));
		sun->transform->SetPosition(-3.0f * sun->transform->GetForward());
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("arrowEdgy");
		meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
		meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
		meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("texture", TextureManager::GetTexture2d("wall1"));
		sun->AddComponent<MeshRenderer>(meshRenderer);
		DirectionalLight* directionalLight = new DirectionalLight(Float4(1.0f, 1.0f, 1.0f, 1.0f));
		sun->AddComponent<DirectionalLight>(directionalLight);
		scene->AddGameObject(sun);
    }
    { // Floor:
        GameObject* floor = new GameObject("floor");
        floor->transform->SetPosition(Float3(0.0f));
        floor->transform->SetScale(8.0f);
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitQuad");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("texture", TextureManager::GetTexture2d("grass"));
        floor->AddComponent<MeshRenderer>(meshRenderer);
        scene->AddGameObject(floor);
    }
    {// Cube0:
        GameObject* cube = new GameObject("cube0");
        cube->transform->SetPosition(Float3(1.5f, 0.0f, 0.5f));
        cube->transform->SetEulerDegrees(Float3(0.0f, 0.0f, 45.0f));
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("texture", TextureManager::GetTexture2d("brick"));
        cube->AddComponent<MeshRenderer>(meshRenderer);

        Spin* spin = new Spin(Float3(0.0f, 0, 45.0f));
		cube->AddComponent<Spin>(spin);

        scene->AddGameObject(cube);
    }
    {// Cube1:
        GameObject* cube = new GameObject("cube1");
        cube->transform->SetPosition(Float3(-1.75f, 1.0f, 0.5f));
        cube->transform->SetEulerDegrees(Float3(0.0f, 0.0f, 30.0f));
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("texture", TextureManager::GetTexture2d("stones"));
        cube->AddComponent<MeshRenderer>(meshRenderer);
        Spin* spin = new Spin(Float3(0.0f, 45.0f, 0.0f));
        cube->AddComponent<Spin>(spin);
        scene->AddGameObject(cube);
    }
    {// Sphere:
        GameObject* sphere = new GameObject("sphere0");
        sphere->transform->SetPosition(Float3(-0.5f, -1.0f, 1.5f));
        sphere->transform->SetEulerDegrees(Float3(0.0f, 0.0f, 0.0f));
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("cubeSphere");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("texture", TextureManager::GetTexture2d("wall0"));
        sphere->AddComponent<MeshRenderer>(meshRenderer);
        Spin* spin = new Spin(Float3(90.0f, 0.0f, 0.0f));
        sphere->AddComponent<Spin>(spin);
        scene->AddGameObject(sphere);
    }
    {// ThreeLeg:
        GameObject* threeLeg = new GameObject("threeLeg");
        threeLeg->transform->SetPosition(Float3(1.5f, -1.0f, 1.5f));
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("colorMaterial"));
        threeLeg->AddComponent<MeshRenderer>(meshRenderer);
        scene->AddGameObject(threeLeg);
    }
    { // Quad:
        GameObject* quad = new GameObject("quad");
        quad->transform->SetPosition(Float3(1.0f, -2.0f, 1.0f));
        quad->transform->SetEulerDegrees(Float3(90.0f, 0.0f, 0.0f));
        quad->transform->SetScale(2.0f);
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitQuad");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("shadowMapMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("texture", app.renderer->shadowRenderPass->shadowMap.get());
        quad->AddComponent<MeshRenderer>(meshRenderer);
        scene->AddGameObject(quad);
    }
    app.SetScene(scene);



    // Debugging:
    //MeshManager::PrintAllMeshNames();
    //MaterialManager::PrintAllMaterialNames();
    //scene->PrintAllGameObjects();
    //scene->PrintAllMeshRenderers();



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