#pragma once
#include "application.h"
#include "emberEngine.h"



// Encapsulate in emberEngine namespace to avoid main.cpp namespace polution due to using namespace emberEngine.
namespace emberEngine
{
	class TEST_GpuSort : public ::testing::Test
	{
	public:
		static void SetUpTestSuite()
		{
			// Initialization:
			emberApplication::Application::CreateInfo appCreateInfo = {};
			appCreateInfo.vSyncEnabled = false;
			appCreateInfo.framesInFlight = 1;
			appCreateInfo.msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount01;
			appCreateInfo.windowWidth = 1600;
			appCreateInfo.windowHeight = 900;
			appCreateInfo.renderWidth = 1600;
			appCreateInfo.renderHeight = 900;
			emberApplication::Application::Init(appCreateInfo);
		}
		static void TearDownTestSuite()
		{
			emberApplication::Application::Clear();
		}
	};



	TEST_F(TEST_GpuSort, GetDataFromGpu)
	{
		int count = 16;
		uint64_t size = (uint64_t)count * (uint64_t)sizeof(float);
	
		// Prepare cpu arrays:
		std::vector<float> uploadData;
		std::vector<float> downloadData;
		uploadData.resize(count);
		downloadData.resize(count);
		for (int i = 0; i < count; i++)
			uploadData[i] = (float)i;
	
		// GPU upload and download:
		Buffer buffer = Buffer(count, sizeof(float), "dataBuffer", BufferUsage::storage);
		buffer.Upload(uploadData.data(), count * sizeof(float));
		buffer.Download(downloadData.data(), count * sizeof(float));
	
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



	TEST_F(TEST_GpuSort, LocalBitonicSort)
	{
		// Create storage buffer:
		int count = 111;	// must be <= BLOCK_SIZE of localBitonicSort compute shader.
		uint32_t size = count * sizeof(int);
		Buffer buffer = Buffer(count, sizeof(int), "dataBuffer", BufferUsage::storage);
	
		// Create unsorted array and upload to buffer:
		std::vector<int> unsortedData;
		unsortedData.resize(count);
		for (int i = 0; i < count; i++)
			unsortedData[i] = math::Random::Uniform(1, count);
		buffer.Upload(unsortedData.data(), count * sizeof(int));
	
		// Sort array on CPU:
		std::vector<int> sortedDataCpu = math::CopySort(unsortedData, [](int a, int b) { return a < b; });
	
		// Load compute shader and set shaderProperties:
		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		ComputeShader sortCS = ComputeShader("localBitonicSort", directoryPath / "localBitonicSort.comp.spv");
		ShaderProperties shaderProperties = ShaderProperties(sortCS);
		shaderProperties.SetBuffer("dataBuffer", buffer);
		shaderProperties.SetValue("Values", "bufferSize", count);
		
		// Dispatch compute shader and wait for finish:
		Uint3 threadCount(count / 2, 1, 1);
		Compute::Immediate::Dispatch(sortCS, shaderProperties, threadCount);
	
		// Download sorted data from GPU:
		std::vector<int> sortedDataGpu;
		sortedDataGpu.resize(count);
		buffer.Download(sortedDataGpu.data(), count * sizeof(int));
	
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



	TEST_F(TEST_GpuSort, BitonicSort)
	{
		// Create storage buffer:
		int count = 1234567;
		uint32_t size = count * sizeof(int);
		Buffer buffer = Buffer(count, sizeof(int), "dataBuffer", BufferUsage::storage);
	
		// Create unsorted array and upload to buffer:
		std::vector<int> unsortedData;
		unsortedData.resize(count);
		math::Random::SetSeed(0);
		for (int i = 0; i < count; i++)
			unsortedData[i] = math::Random::Uniform(1, count);
		buffer.Upload(unsortedData.data(), count * sizeof(int));
	
		// Sort array on CPU:
		Time::Reset();
		std::vector<int> sortedDataCpu = math::CopySort(unsortedData, [](int a, int b) { return a < b; });
		Time::Update();
		LOG_INFO("CPU sort time: {}s", Time::GetDeltaTime());
	
		// Sort array on GPU:
		GpuSort::SortImmediate(buffer);
		Time::Update();
		LOG_INFO("GPU sort time: {}s", Time::GetDeltaTime());
	
		// Download sorted data from GPU:
		std::vector<int> sortedDataGpu;
		sortedDataGpu.resize(count);
		buffer.Download(sortedDataGpu.data(), count * sizeof(int));
	
		// Print results:
		//for (int i = 0; i < count; i++)
		//{
		//	//LOG_TRACE(sortedDataGpu[i]);
		//	if (sortedDataCpu[i] == sortedDataGpu[i])
		//		LOG_TRACE("index: {}, unsorted:{}, CPU:{}, GPU:{}", i, unsortedData[i], sortedDataCpu[i], sortedDataGpu[i]);
		//	else
		//		LOG_WARN("index: {}, unsorted:{}, CPU:{}, GPU:{}", i, unsortedData[i], sortedDataCpu[i], sortedDataGpu[i]);
		//}
	
		// Check results:
		bool allGood = true;
		for (int i = 0; i < count; i++)
		{
			allGood = (sortedDataGpu[i] == sortedDataCpu[i]);
			if (allGood == false)
				break;
		}
		EXPECT_TRUE(allGood);
	}

	//Scene* InitScene()
	//{
	//	Scene* pScene = new Scene();
	//	{// Camera:
	//		GameObject* pGameObject = new GameObject("mainCamera");
	//		Float3 pos = Float3(0.0f, -5.0f, 0.0f);
	//		pGameObject->GetTransform()->SetPosition(pos);
	//		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));
	//
	//		Camera* pCamera = pGameObject->AddComponent<Camera>();
	//		pCamera->SetFarClip(100.0f);
	//		pCamera->SetProjectionType(ProjectionType::orthographic);
	//		pCamera->SetOrthographicSize(6);
	//		CameraController* cameraController = pGameObject->AddComponent<CameraController>();
	//		
	//		pScene->AddGameObject(pGameObject);
	//		pScene->SetActiveCamera(pCamera);
	//	}
	//	{// Bitonic sort graph:
	//		GameObject* pGameObject = new GameObject("bitonic sort graph");
	//		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateX(math::pi2));
	//		BitonicSortGraph* pBitonicSortGraph = pGameObject->AddComponent<BitonicSortGraph>();
	//		pScene->AddGameObject(pGameObject);
	//	}
	//	return pScene;
	//}
	// Not a real test. Only for plotting of the bitonic sort graph.
	//TEST_F(TEST_GpuSort, Visualization)
	//{
	//	// Create scene:
	//	std::unique_ptr<Scene> pScene(InitScene());
	//	TEST_GpuSort::app->SetScene(pScene.get());
	//
	//	// Run application:
	//	try
	//	{
	//		TEST_GpuSort::app->Run();
	//	}
	//	catch (const std::exception& e)
	//	{
	//		LOG_ERROR("Exception: {}", e.what());
	//	}
	//}
}