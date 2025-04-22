#ifndef __INCLUDE_GUARD_testGpuSort_h__
#define __INCLUDE_GUARD_testGpuSort_h__
#include "application.h"
#include "bitonicSort.h"
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
#include "postRenderEffects.h"
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
			settings.renderWidth = 2 * 1920; //1280;
			settings.renderHeight = 2 * 1080; //720;
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
		stagingBufferA.UploadToBuffer(&stagingBufferB, vulkanBackend::Context::pLogicalDevice->GetTransferQueue());
	
		// Download data from stagingBuffer B:
		std::vector<float> downloadData;
		downloadData.resize(count);
		stagingBufferB.DownloadFromBuffer(&stagingBufferA, vulkanBackend::Context::pLogicalDevice->GetTransferQueue());
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



	TEST_F(GpuSort, LocalBitonicSort)
	{
		// Create storage buffer:
		int count = 111;	// must be <= BLOCK_SIZE of localBitonicSort compute shader.
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
		uploadBuffer.UploadToBuffer(&buffer, vulkanBackend::Context::pLogicalDevice->GetTransferQueue());
	
		// Sort array on CPU:
		std::vector<int> sortedDataCpu = math::CopySort(unsortedData, [](int a, int b) { return a < b; });
	
		// Load compute shader and set shaderProperties:
		std::string directoryPath = (std::string)ENGINE_ROOT_PATH + "/src/shaders/bin";
		ComputeShader sortCS = ComputeShader("localBitonicSort", directoryPath + "/localBitonicSort.comp.spv");
		ShaderProperties shaderProperties = ShaderProperties((Shader*)&sortCS);
		shaderProperties.SetStorageBuffer("dataBuffer", &buffer);
		shaderProperties.SetValue("Values", "bufferSize", count);
		
		// Dispatch compute shader and wait for finish:
		Uint3 threadCount(count / 2, 1, 1);
		compute::Immediate::Dispatch(&sortCS, &shaderProperties, threadCount);
	
		// Download sorted data from GPU:
		std::vector<int> sortedDataGpu;
		sortedDataGpu.resize(count);
		StagingBuffer downloadBuffer(size);
		downloadBuffer.DownloadFromBuffer(&buffer, vulkanBackend::Context::pLogicalDevice->GetTransferQueue());
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
		EXPECT_TRUE(allGood);
	}

	TEST_F(GpuSort, MultiSort)
	{
		// Create storage buffer:
		int count = 123893;
		uint32_t size = count * sizeof(int);
		StorageBuffer buffer = StorageBuffer((uint32_t)count, (uint32_t)sizeof(int));
	
		// Create unsorted array:
		std::vector<int> unsortedData;
		unsortedData.resize(count);
		math::Random::SetSeed(0);
		for (int i = 0; i < count; i++)
			unsortedData[i] = math::Random::Uniform(1, count);
	
		// Upload unsorted data to GPU:
		StagingBuffer uploadBuffer(size);
		uploadBuffer.SetData(unsortedData.data(), size);
		uploadBuffer.UploadToBuffer(&buffer, vulkanBackend::Context::pLogicalDevice->GetTransferQueue());
	
		// Sort array on CPU:
		std::vector<int> sortedDataCpu = math::CopySort(unsortedData, [](int a, int b) { return a < b; });

		// Dispatch compute shaders:
		BitonicSort::Sort(&buffer);
	
		// Download sorted data from GPU:
		std::vector<int> sortedDataGpu;
		sortedDataGpu.resize(count);
		StagingBuffer downloadBuffer(size);
		downloadBuffer.DownloadFromBuffer(&buffer, vulkanBackend::Context::pLogicalDevice->GetTransferQueue());
		downloadBuffer.GetData(sortedDataGpu.data(), size);
	
		//// Print data:
		//for (int i = 0; i < count; i++)
		//{
		//	//LOG_TRACE(sortedDataGpu[i]);
		//	if (sortedDataCpu[i] == sortedDataGpu[i])
		//		LOG_TRACE("index: {}, unsorted:{}, CPU:{}, GPU:{}", i, unsortedData[i], sortedDataCpu[i], sortedDataGpu[i]);
		//	else
		//		LOG_WARN("index: {}, unsorted:{}, CPU:{}, GPU:{}", i, unsortedData[i], sortedDataCpu[i], sortedDataGpu[i]);
		//}
	
		// Check if data is correct:
		bool allGood = true;
		for (int i = 0; i < count; i++)
		{
			allGood = (sortedDataGpu[i] == sortedDataCpu[i]);
			if (allGood == false)
				break;
		}
		EXPECT_TRUE(allGood);
	}

	Scene* InitScene()
	{
		Scene* pScene = new Scene();
		{// Camera:
			GameObject* pGameObject = new GameObject("mainCamera");
			Float3 pos = Float3(0.0f, -5.0f, 0.0f);
			pGameObject->GetTransform()->SetPosition(pos);
			pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));
	
			Camera* pCamera = pGameObject->AddComponent<Camera>();
			pCamera->SetFarClip(100.0f);
			pCamera->SetProjectionType(ProjectionType::orthographic);
			pCamera->SetOrthographicSize(6);
			CameraController* cameraController = pGameObject->AddComponent<CameraController>();
			
			pScene->AddGameObject(pGameObject);
			pScene->SetActiveCamera(pCamera);
		}
		{// Bitonic sort graph:
			GameObject* pGameObject = new GameObject("bitonic sort graph");
			pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateX(math::pi2));
			BitonicSortGraph* pBitonicSortGraph = pGameObject->AddComponent<BitonicSortGraph>();
			pScene->AddGameObject(pGameObject);
		}
		return pScene;
	}
	// Not a real test. Only for plotting of the bitonic sort graph.
	//TEST_F(GpuSort, Visualization)
	//{
	//	// Create scene:
	//	std::unique_ptr<Scene> pScene(InitScene());
	//	GpuSort::app->SetScene(pScene.get());
	//
	//	// Run application:
	//	try
	//	{
	//		GpuSort::app->Run();
	//	}
	//	catch (const std::exception& e)
	//	{
	//		LOG_ERROR("Exception: {}", e.what());
	//	}
	//}
}



#endif // __INCLUDE_GUARD_testGpuSort_h__