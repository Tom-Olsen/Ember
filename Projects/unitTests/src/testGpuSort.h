#ifndef __INCLUDE_GUARD_testGpuSort_h__
#define __INCLUDE_GUARD_testGpuSort_h__
#include "application.h"
#include "compute.h"
#include "computeShader.h"
#include "emberMath.h"
#include "shaderProperties.h"
#include "stagingBuffer.h"
#include "storageBuffer.h"
#include "vulkanContext.h"
#include <string>

// Components:
#include "bitonicSortGraph.h"
#include "camera.h"
#include "cameraController.h"
#include "directionalLight.h"
#include "meshRenderer.h"
#include "pointLight.h"
#include "postProcessEffects.h"
#include "spotLight.h"
#include "transform.h"


// Encapsulate in namespace to avoid main.cpp namespace polution.
namespace emberEngine
{
	class GpuSort : public ::testing::Test
	{
	public:
		static Application* app;
		static void SetUpTestSuite()
		{
			Application::Settings settings = {};
			settings.vSyncEnabled = true;
			settings.framesInFlight = 2;
			settings.msaaSamples = VK_SAMPLE_COUNT_4_BIT;
			settings.windowWidth = 1920;
			settings.windowHeight = 1080;
			settings.renderWidth = 1280;
			settings.renderheight = 720;
			app = new Application(settings);
		}
		static void TearDownTestSuite()
		{
			delete app;
		}
	};
	Application* GpuSort::app = nullptr;



	TEST_F(GpuSort, GetDataFromGpu)
	{
		int count = 16;
		uint64_t size = (uint64_t)count * (uint64_t)sizeof(float);

		// Set upload data on CPU:
		std::vector<float> uploadData;
		uploadData.resize(count);
		for (int i = 0; i < count; i++)
			uploadData[i] = (float)i;

		// Perpare two staging buffers:
		StagingBuffer stagingBufferA(size);
		StagingBuffer stagingBufferB(size);

		// Upload data from stagingBuffer A to B:
		stagingBufferA.SetData(uploadData.data(), size);
		stagingBufferA.UploadToBuffer(stagingBufferB.GetVmaBuffer(), VulkanContext::pLogicalDevice->GetTransferQueue());

		// Download data from stagingBuffer B:
		std::vector<float> downloadData;
		downloadData.resize(count);
		stagingBufferB.DownloadFromBuffer(stagingBufferA.GetVmaBuffer(), VulkanContext::pLogicalDevice->GetTransferQueue());
		stagingBufferB.GetData(downloadData.data(), size);

		// Check if data is correct:
		bool allGood = true;
		for (int i = 0; i < count; i++)
		{
			allGood = (downloadData[i] == uploadData[i]);
			if (allGood == false)
				break;
		}
		EXPECT_TRUE(allGood);
	}



	TEST_F(GpuSort, SingleSort)
	{
		// Create storage buffer:
		int count = 653;	// must be <= COUNT of bitonicSort compute shader
		uint32_t size = count * sizeof(int);
		StorageBuffer buffer = StorageBuffer((uint32_t)count, (uint32_t)sizeof(int));

		// Create unsorted array:
		std::vector<int> unsortedData;
		unsortedData.resize(count);
		for (int i = 0; i < count; i++)
			unsortedData[i] = math::Random::Uniform(1, count);

		// Upload unsorted data to GPU:
		StagingBuffer uploadBuffer(size);
		uploadBuffer.SetData(unsortedData.data(), size);
		uploadBuffer.UploadToBuffer(buffer.GetVmaBuffer(), VulkanContext::pLogicalDevice->GetTransferQueue());

		// Sort array on CPU:
		std::vector<int> sortedDataCpu = math::CopySort(unsortedData, [](int a, int b) { return a < b; });

		// Load compute shader and set shaderProperties:
		std::string directoryPath = (std::string)ENGINE_ROOT_PATH + "/src/shaders/bin";
		ComputeShader sortCS = ComputeShader("bitonicSort", directoryPath + "/bitonicSort.comp.spv");
		ShaderProperties shaderProperties = ShaderProperties((Shader*)&sortCS);
		shaderProperties.SetStorageBuffer("sortBuffer", &buffer);
		shaderProperties.SetValue("Values", "bufferSize", count);
		
		// Dispatch compute shader:
		Uint3 threadCount(count / 2, 1, 1);
		Compute::DispatchImmediatly(&sortCS, &shaderProperties, threadCount);

		// Download sorted data from GPU:
		std::vector<int> sortedDataGpu;
		sortedDataGpu.resize(count);
		StagingBuffer downloadBuffer(size);
		downloadBuffer.DownloadFromBuffer(buffer.GetVmaBuffer(), VulkanContext::pLogicalDevice->GetTransferQueue());
		downloadBuffer.GetData(sortedDataGpu.data(), size);

		//// Print data:
		//for (int i = 0; i < count; i++)
		//	LOG_TRACE("unsorted:{}, CPU:{}, GPU:{}", unsortedData[i], sortedDataCpu[i], sortedDataGpu[i]);

		// Check if data is correct:
		bool allGood = true;
		for (int i = 0; i < count; i++)
		{
			allGood = (sortedDataGpu[i] == sortedDataCpu[i]);
			if (allGood == false)
				break;
		}
		LOG_WARN(allGood);
		EXPECT_TRUE(allGood);
	}

	void DispatchSingleSort(int offset, int bufferSize, StorageBuffer* buffer)
	{
		static bool isInitialized = false;
		static const std::string directoryPath = (std::string)ENGINE_ROOT_PATH + "/src/shaders/bin";
		static ComputeShader* computeShader;
		static ShaderProperties* shaderProperties;
		if (!isInitialized)
		{
			computeShader = new ComputeShader("bitonicSort", directoryPath + "/bitonicSort.comp.spv");
			shaderProperties = new ShaderProperties((Shader*)computeShader);
		}
		shaderProperties->SetStorageBuffer("sortBuffer", buffer);
		shaderProperties->SetValue("Values", "offset", offset);
		shaderProperties->SetValue("Values", "bufferSize", bufferSize);

		Uint3 threadCount(bufferSize / 2, 1, 1);
		Compute::DispatchImmediatly(computeShader, shaderProperties, threadCount);
	}

	TEST_F(GpuSort, MultiSort)
	{
		// Create storage buffer:
		int count = 20;
		uint32_t size = count * sizeof(int);
		StorageBuffer buffer = StorageBuffer((uint32_t)count, (uint32_t)sizeof(int));

		// Create unsorted array:
		std::vector<int> unsortedData;
		unsortedData.resize(count);
		for (int i = 0; i < count; i++)
			unsortedData[i] = math::Random::Uniform(1, count);

		// Upload unsorted data to GPU:
		StagingBuffer uploadBuffer(size);
		uploadBuffer.SetData(unsortedData.data(), size);
		uploadBuffer.UploadToBuffer(buffer.GetVmaBuffer(), VulkanContext::pLogicalDevice->GetTransferQueue());

		// Sort array on CPU:
		std::vector<int> sortedDataCpu = math::CopySort(unsortedData, [](int a, int b) { return a < b; });

		// Dispatch compute shaders:
		int batchSize = 16;
		int batches = (count - 1) / batchSize + 1;
		int remaining = count;
		int bufferSize = batchSize;
		// For now each dispatch sorts only its own subarray of batchSize elements (last one can have fever elements).
		for (int i = 0; i < batches; i++)
		{
			if (remaining < batchSize)
				bufferSize = remaining;
			else
				remaining -= batchSize;
			DispatchSingleSort(i * batchSize, bufferSize, &buffer);
		}

		// Download sorted data from GPU:
		std::vector<int> sortedDataGpu;
		sortedDataGpu.resize(count);
		StagingBuffer downloadBuffer(size);
		downloadBuffer.DownloadFromBuffer(buffer.GetVmaBuffer(), VulkanContext::pLogicalDevice->GetTransferQueue());
		downloadBuffer.GetData(sortedDataGpu.data(), size);

		// Print data:
		for (int i = 0; i < count; i++)
			LOG_TRACE("unsorted:{}, CPU:{}, GPU:{}", unsortedData[i], sortedDataCpu[i], sortedDataGpu[i]);

		// Check if data is correct:
		bool allGood = true;
		for (int i = 0; i < count; i++)
		{
			allGood = (sortedDataGpu[i] == sortedDataCpu[i]);
			if (allGood == false)
				break;
		}
		LOG_WARN(allGood);
		EXPECT_TRUE(allGood);
	}

	Scene* InitScene()
	{
		Scene* pScene = new Scene();
		{// Camera:
			GameObject* pGameObject = new GameObject("mainCamera");
			Float3 pos = Float3(0.0f, -5.0f, 1.0f);
			pGameObject->GetTransform()->SetPosition(pos);
			pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));
	
			Camera* pCamera = pGameObject->AddComponent<Camera>();
			pCamera->SetFarClip(100.0f);
			CameraController* cameraController = pGameObject->AddComponent<CameraController>();
			
			pScene->AddGameObject(pGameObject);
			pScene->SetActiveCamera(pCamera);
		}
		{// Skybox:
			GameObject* pGameObject = new GameObject("skybox");
			pGameObject->GetTransform()->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

			MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
			pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
			pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
			pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
			pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("skyBox0"));
			pMeshRenderer->SetCastShadows(false);
			pMeshRenderer->SetReceiveShadows(false);
	
			pScene->AddGameObject(pGameObject);
		}
		{// Bitonic sort graph:
			GameObject* pGameObject = new GameObject("bitonic sort graph");
			BitonicSortGraph* pBitonicSortGraph = pGameObject->AddComponent<BitonicSortGraph>();
			pScene->AddGameObject(pGameObject);
		}
		return pScene;
	}
	TEST_F(GpuSort, Visualization)
	{
		// Create scene:
		std::unique_ptr<Scene> pScene(InitScene());
		GpuSort::app->SetScene(pScene.get());
	
		// Run application:
		try
		{
			GpuSort::app->Run();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
	}
}



#endif // __INCLUDE_GUARD_testGpuSort_h__