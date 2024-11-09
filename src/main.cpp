#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"



// TODO now:
// - refactor code and make everything cleaner!
// - better lighting model (specular highlights etc.)

// TODO long term:
// - proper quaternion support
// - reduce normalMatrix to 3x3 matrix in transform and shader
// - 3d ui renderpass that draws on top of everything and is not affected by the camera (constant view/projection matrix)
// - input system (get inputs from SdlWindow->HandlEvents() and send them to the active scene)
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
// - uniform buffer types hard coded via binding name. make it dynamic
// - change shared ptr in VulkanUniformBuffer.buffer to unique ptr
// - support arrays in spirv reflection
//`- remove unnecessary includes (iostream)
// - write own logger class
// - better shadow mapping (PCF, soft shadows, etc.)
// - replace all sin/cos/tan/asin/acos/atan/atan2 with custom float versions



#include <fstream>
std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile)
{
    // Open shader file:
    std::ifstream file(spvFile, std::ios::binary);
    if (!file.is_open())
        LOG_CRITICAL("Error opening shader file: {}", spvFile.string());

    // Get file size:
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    // Copy code:
    std::vector<char> code(fileSize);
    file.read(code.data(), fileSize);
    file.close();

    return code;
}



int main()
{
    // VS debugging:
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Initialization:
    Logger::Init();
    Application app;
    //MaterialManager::GetMaterial("defaultMaterial")->PrintUniformBuffers();
    //return 0;

    ShadowRenderPass* shadowRenderPass = dynamic_cast<ShadowRenderPass*>(RenderPassManager::GetRenderPass("shadowRenderPass"));

    // Build simple scene:
    Scene* scene = new Scene();
    {// Camera:
        GameObject* camera = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, 3.0f, 5.0f);
        camera->transform->SetPosition(pos);
        camera->transform->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up));
    
        Camera* cameraComponent = new Camera();
        camera->AddComponent<Camera>(cameraComponent);
    
        //Spin* spin = new Spin(Float3(60.0f, 00.0f, 00.0f));
        //camera->AddComponent<Spin>(spin);
    
        scene->AddGameObject(camera);
		scene->SetActiveCamera(cameraComponent);
    }
    {// Sun:
        GameObject* sun = new GameObject("sun");
        Float3 pos = Float3(7.0f, 7.0f, 3.5f);
		sun->transform->SetPosition(pos);
        Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
        sun->transform->SetRotationMatrix(matrix);
    
        //Mesh* mesh = MeshManager::GetMesh("threeLeg");
        //MeshRenderer* meshRenderer = new MeshRenderer();
        //meshRenderer->mesh = mesh;
		//meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
		//meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("colorMaterial"));
		//sun->AddComponent<MeshRenderer>(meshRenderer);
    
		DirectionalLight* directionalLight = new DirectionalLight(Float4(1.0f, 1.0f, 1.0f, 1.0f));
		sun->AddComponent<DirectionalLight>(directionalLight);
    
        LOG_INFO("Sun position: {}", sun->transform->GetPosition().ToString());
		LOG_INFO("Sun view direction: {}", sun->transform->GetBackward().ToString());
        LOG_INFO("Sun localToWorld: {}", sun->transform->GetLocalToWorldMatrix().ToString());
        LOG_INFO("Sun worldToLocal: {}", sun->transform->GetWorldToLocalMatrix().ToString());
		LOG_INFO("Sun projection: {}", directionalLight->GetProjectionMatrix().ToString());
    
		scene->AddGameObject(sun);
    }
    { // Floor:
        GameObject* floor = new GameObject("floor");
        floor->transform->SetPosition(0.0f, -0.5f, 0.0f);
        floor->transform->SetScale(8.0f);
        floor->transform->SetRotationEulerDegrees(-90.0f, 0.0f, 0.0f);

        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitQuad");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", TextureManager::GetTexture2d("grass"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("shadowMap", shadowRenderPass->shadowMap.get());
        floor->AddComponent<MeshRenderer>(meshRenderer);

        //Spin* spin = new Spin(Float3(0.0f, 0.0f, 60.0f));
        //floor->AddComponent<Spin>(spin);
    
        scene->AddGameObject(floor);
    }
    {// Cube0:
        GameObject* cube = new GameObject("cube0");
        cube->transform->SetPosition(-1.0f, 0.0f, -1.0f);
        cube->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", TextureManager::GetTexture2d("brick"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("shadowMap", shadowRenderPass->shadowMap.get());
        cube->AddComponent<MeshRenderer>(meshRenderer);
    
        Spin* spin = new Spin(Float3(0.0f, 45.0f, 0.0f));
        cube->AddComponent<Spin>(spin);
    
        scene->AddGameObject(cube);
    }
    {// Cube1:
        GameObject* cube = new GameObject("cube1");
        cube->transform->SetPosition(2.0f, 0.0f, 0.0f);
        cube->transform->SetRotationEulerDegrees(0.0f, 20.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", TextureManager::GetTexture2d("stones"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("shadowMap", shadowRenderPass->shadowMap.get());
        cube->AddComponent<MeshRenderer>(meshRenderer);
    
        //Spin* spin = new Spin(Float3(0.0f, 45.0f, 0.0f));
        //cube->AddComponent<Spin>(spin);
    
        scene->AddGameObject(cube);
    }
    {// Sphere:
        GameObject* sphere = new GameObject("sphere0");
        sphere->transform->SetPosition(0.5f, 0.0f, 1.5f);
        sphere->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("cubeSphere");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", TextureManager::GetTexture2d("wall0"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("shadowMap", shadowRenderPass->shadowMap.get());
        sphere->AddComponent<MeshRenderer>(meshRenderer);
    
        //Spin* spin = new Spin(Float3(90.0f, 0.0f, 0.0f));
        //sphere->AddComponent<Spin>(spin);
    
        scene->AddGameObject(sphere);
    }
    {// ThreeLeg:
        GameObject* threeLeg = new GameObject("threeLeg");
        threeLeg->transform->SetPosition(-2.0f, 0.0f, 1.0f);
		threeLeg->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        meshRenderer->SetShadowMaterial(MaterialManager::GetMaterial("shadowMaterial"));
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("colorMaterial"));
        threeLeg->AddComponent<MeshRenderer>(meshRenderer);
    
        //Spin* spin = new Spin(Float3(0.0f, 0.0f, 20.0f));
        //threeLeg->AddComponent<Spin>(spin);
    
        scene->AddGameObject(threeLeg);
    }
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