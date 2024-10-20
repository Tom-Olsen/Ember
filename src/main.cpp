#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"



int main()
{
    // VS debugging:
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Initialization:
    Logger::Init();
    uint32_t framesInFlight = 2;
	std::shared_ptr<VulkanContext> context = std::make_shared<VulkanContext>(framesInFlight);
    Application app(context);
    MeshManager::Init(context.get());
    MaterialManager::Init(context.get());
	TextureManager::Init(context.get());
	SamplerManager::Init(context.get());

    // Build simple scene:
    Scene* scene = new Scene();
    {// Camera:
        GameObject* camera = new GameObject("mainCamera");
        camera->transform->SetPosition(Float3(0.0f, -2.0f, 2.0f));
        camera->transform->SetEulerAngles(Float3(45.0f, 0.0f, 0.0f));
        Camera* cameraComponent = new Camera();
        camera->AddComponent<Camera>(cameraComponent);
        scene->AddGameObject(camera);
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



    // Cleanup:
	SamplerManager::Clear();
	TextureManager::Clear();
    MaterialManager::Clear();
    MeshManager::Clear();
    delete scene;

    return 0;
}