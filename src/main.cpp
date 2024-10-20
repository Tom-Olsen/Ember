#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"



void Set(std::vector<int>&& in, std::vector<int>& b)
{
    b = in;
}
void Test()
{
    Mesh mesh("test");

    std::vector<Float3> positions;
    positions.emplace_back(-0.5f, -0.5f, 0.0f);
    positions.emplace_back(-0.5f, 0.5f, 0.0f);
    positions.emplace_back(0.5f, -0.5f, 0.0f);
    positions.emplace_back(0.5f, 0.5f, 0.0f);
    //mesh.MovePositions(positions);
    mesh.SetPositions(positions);

    LOG_INFO("positions: {}", positions.size());
    for (Float3 i : positions)
		LOG_INFO(to_string(i));

    LOG_INFO("\nmesh.positions: {}", mesh.GetPositions().size());
	for (Float3 i : mesh.GetPositions())
		LOG_INFO(to_string(i));

    int k;
    std::cin >> k;
}



int main()
{
    // VS debugging:
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Initialization:
    Logger::Init();
    uint32_t framesInFlight = 2;
	std::shared_ptr<VulkanContext> context = std::make_shared<VulkanContext>(framesInFlight);
    Application app(context);
    MeshManager::InitDefaultMeshes(context.get());
    MaterialManager::InitDefaultMaterials(context.get());


    // TODO: move these into own manager classes:
    std::unique_ptr<VulkanSampler> sampler;
    std::unique_ptr<Texture2d> texture2d;
    sampler = std::make_unique<VulkanSampler>(app.context.get(), "defaultSampler");
    texture2d = std::make_unique<Texture2d>(app.context.get(), "../textures/example.jpg", "example");

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
        meshRenderer->materialProperties->SetSamplerForAllFrames("samplerState", sampler.get());
        meshRenderer->materialProperties->SetTexture2dForAllFrames("texture", texture2d.get());
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
        meshRenderer->materialProperties->SetSamplerForAllFrames("samplerState", sampler.get());
        meshRenderer->materialProperties->SetTexture2dForAllFrames("texture", texture2d.get());
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
        meshRenderer->materialProperties->SetSamplerForAllFrames("samplerState", sampler.get());
        meshRenderer->materialProperties->SetTexture2dForAllFrames("texture", texture2d.get());
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
    MaterialManager::ClearMaterialMap();
    MeshManager::ClearMeshMap();
    delete scene;

    return 0;
}