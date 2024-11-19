#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"
#include<iostream>



// TODO now:
// - validation layer error when gameobject name is "direcionalLight1" wtf?
// - give the material a default materialProperties which is used if no materialProperties are set
// - move Material.GetDescriptorSetLayoutBindings function to spirvReflect class
// - rethink UniformBufferBlock submembers structure:
//   - dont use subMembers at all and have each structmember be a member of the block?
//   - same for array entries?
// - refactor code and make everything cleaner!
// - better lighting model (specular highlights etc.)
// - add gameObject selection (need gizmos => ui renderpass)
// - text rendering
// - blender model import
// - currently one commandPool per commandBuffer, should be one commandPool per frame, shared by all commands for that frame

// TODO long term:
// - proper quaternion support
// - 3d ui renderpass that draws on top of everything and is not affected by the camera (constant view/projection matrix)
// - render image while resizing
// - implement game physics fixedUpdate loop
// - post processing
// - skybox
// - particles (instancing)
// - physics
// - audio
// - gameobject clipping logic (requires bounding box)
// - change shared ptr in VulkanUniformBuffer.buffer to unique ptr
// - support arrays in spirv reflection
//`- remove unnecessary includes (iostream)
// - write own logger class
// - better shadow mapping (PCF, soft shadows, etc.)
// - engine name?
// - compile engine as .dll and link to game/test project
// - imgui integration
// - render into custom image with higher precision (16-bit floats for color instead of 8-bit) and push final image to swapchain.
//   This allows for HDR rendering and post processing effects.

// FIXME:
// - in materialProperties the unordered map updateUniformBuffer needs to track the frame index for each uniform buffer
//   and update the uniform buffer continuously until all frames have the correct data.
//   Similar thing would be useful for texture and sampler maps. This would allow to remove the SetTexture2d/SamplerForAllFrames functions.



int main()
{
    // VS debugging:
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Initialization:
    Logger::Init();
    Application app;

    //LOG_TRACE(material->GetUniformBufferBlock("LightMatrizes")->ToString());
    //MaterialManager::GetMaterial("defaultMaterial")->PrintBindings();
    //MaterialManager::GetMaterial("colorMaterial")->PrintBindings();

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
    {// Directional Light0:
        LOG_INFO("Light Direction 0 Creation:");
        GameObject* gameObject = new GameObject("directionalLight0");
        Float3 pos = Float3(7.0f, 7.0f, 3.5f);
        Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
        gameObject->transform->SetPosition(pos);
        gameObject->transform->SetRotationMatrix(matrix);

        //MeshRenderer* meshRenderer = new MeshRenderer();
        //meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        //meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("colorMaterial"));
		//meshRenderer->castShadows = meshRenderer->receiveShadows = false;
        //gameObject->AddComponent<MeshRenderer>(meshRenderer);
        //
        //LOG_TRACE("Setting forwardMaterial to: {}", meshRenderer->GetForwardMaterial()->name);
        //std::cout << "DescriptorSet adresses are:";
        //for (uint32_t i = 0; i < app.context->framesInFlight; i++)
        //    std::cout << meshRenderer->forwardMaterialProperties->descriptorSets[i] << ", ";
        //std::cout << std::endl;
        //meshRenderer->forwardMaterialProperties->PrintMaps();
    
		DirectionalLight* directionalLight = new DirectionalLight(Float4(1.0f, 1.0f, 1.0f, 1.0f));
        gameObject->AddComponent<DirectionalLight>(directionalLight);
    
		scene->AddGameObject(gameObject);
    }
    {// Directional Light1:
        GameObject* gameObject = new GameObject("directionalLight1");
        Float3 pos = Float3(-7.0f, 7.0f, 3.5f);
        Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
        gameObject->transform->SetPosition(pos);
        gameObject->transform->SetRotationMatrix(matrix);
    
        DirectionalLight* directionalLight = new DirectionalLight(Float4(1.0f, 1.0f, 1.0f, 1.0f));
        gameObject->AddComponent<DirectionalLight>(directionalLight);
    
        scene->AddGameObject(gameObject);
    }
    { // Floor:
        LOG_INFO("Floor Creation: defaultMaterial");
        GameObject* gameObject = new GameObject("floor");
        gameObject->transform->SetPosition(0.0f, -0.5f, 0.0f);
        gameObject->transform->SetScale(8.0f);
        gameObject->transform->SetRotationEulerDegrees(-90.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitQuad");
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", TextureManager::GetTexture2d("grass"));
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
    }
    {// ThreeLeg:
        GameObject* gameObject = new GameObject("threeLeg");
        gameObject->transform->SetPosition(-2.0f, 0.0f, 1.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("threeLeg");
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("colorMaterial"));
        meshRenderer->castShadows = meshRenderer->receiveShadows = false;
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
    }
    {// Cube0:
        GameObject* gameObject = new GameObject("cube0");
        gameObject->transform->SetPosition(-1.0f, 0.0f, -1.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", TextureManager::GetTexture2d("brick"));
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        Spin* spin = new Spin(Float3(0.0f, 45.0f, 0.0f));
        gameObject->AddComponent<Spin>(spin);
    
        scene->AddGameObject(gameObject);
    }
    {// Cube1:
        LOG_INFO("Cube 1 Creation:");
        GameObject* gameObject = new GameObject("cube1");
        gameObject->transform->SetPosition(2.0f, 0.0f, 0.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 20.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitCube");
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", TextureManager::GetTexture2d("stones"));
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
    }
    {// Sphere:
        GameObject* gameObject = new GameObject("sphere0");
        gameObject->transform->SetPosition(0.5f, 0.0f, 0.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("cubeSphere");
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", TextureManager::GetTexture2d("wall0"));
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
    }
    {// ShadowMap Quad:
        GameObject* gameObject = new GameObject("quad");
        gameObject->transform->SetPosition(1.0f, 0.25f, 3.0f);
        gameObject->transform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
        gameObject->transform->SetScale(1.5f);

        Texture2d* shadowMap = static_cast<ShadowRenderPass*>(RenderPassManager::GetRenderPass("shadowRenderPass"))->shadowMap.get();
    
        MeshRenderer* meshRenderer = new MeshRenderer();
        meshRenderer->mesh = MeshManager::GetMesh("unitQuad");
        meshRenderer->SetForwardMaterial(MaterialManager::GetMaterial("defaultMaterial"));
        meshRenderer->forwardMaterialProperties->SetSamplerForAllFrames("colorSampler", SamplerManager::GetSampler("colorSampler"));
        meshRenderer->forwardMaterialProperties->SetTexture2dForAllFrames("colorTexture", shadowMap);
        gameObject->AddComponent<MeshRenderer>(meshRenderer);
    
        scene->AddGameObject(gameObject);
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