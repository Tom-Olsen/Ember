#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"



// TODO:
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
// - rename frameBuffers to framebuffers
// - move framebuffers into vulkanRenderpass
// - rename vulkanRenderpass to ??? (defaultPass?) (renderPass?)



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
        camera->transform->SetPosition(Float3(0.0f, -2.0f, 2.0f));
        camera->transform->SetEulerAngles(Float3(45.0f, 0.0f, 0.0f));
        Camera* cameraComponent = new Camera();
        camera->AddComponent<Camera>(cameraComponent);
        scene->AddGameObject(camera);
		scene->SetActiveCamera(cameraComponent);
    }
    { // Floor:
        GameObject* floor = new GameObject("floor");
        floor->transform->SetScale(5.0f);
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("UnitQuad");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
        meshRenderer->materialProperties->SetTexture2dForAllFrames("texture", TextureManager::GetTexture2d("grass"));
        floor->AddComponent<MeshRenderer>(meshRenderer);
        scene->AddGameObject(floor);
    }
    {// Cube:
        GameObject* cube = new GameObject("cube");
        cube->transform->SetPosition(Float3(0.5f, 0.5f, 0.5f));
        cube->transform->SetEulerAngles(Float3(0.0f, 0.0f, 45.0f));
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("UnitCube");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
        meshRenderer->materialProperties->SetTexture2dForAllFrames("texture", TextureManager::GetTexture2d("brick"));
        cube->AddComponent<MeshRenderer>(meshRenderer);
        Spin* spin = new Spin(Float3(0.0f, 0.0f, 45.0f));
		cube->AddComponent<Spin>(spin);
        scene->AddGameObject(cube);
    }
    {// Cube2:
        GameObject* cube = new GameObject("cube2");
        cube->transform->SetPosition(Float3(-0.75f, 0.0f, 0.5f));
        cube->transform->SetEulerAngles(Float3(0.0f, 0.0f, 30.0f));
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("UnitCube");
        meshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->materialProperties->SetSamplerForAllFrames("samplerState", SamplerManager::GetSampler("default"));
        meshRenderer->materialProperties->SetTexture2dForAllFrames("texture", TextureManager::GetTexture2d("stone"));
        cube->AddComponent<MeshRenderer>(meshRenderer);
        scene->AddGameObject(cube);
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