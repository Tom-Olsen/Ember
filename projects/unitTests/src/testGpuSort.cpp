#include "application.h"
#include "emberEngine.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>



using namespace emberEngine;



// This setUp/tearDown runs once for all test belonging to the suite/fixture TEST_GpuSort:
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



// Verify vector sorting and permutation sorting against the same CPU comparator:
template <typename T, typename Compare>
void TestBitonicSortAndPermutationSort(const std::vector<T>& uploadData, Compare compare)
{
	int count = static_cast<int>(uploadData.size());

	// Sort directly:
	std::vector<T> sortedDataCpu = math::CopySort(uploadData, compare);
	BufferTyped<T> sortBuffer = BufferTyped<T>(count, "sortBuffer", BufferUsage::storage);
	sortBuffer.Upload(uploadData.data(), count);
	GpuSort<T>::SortAndWait(sortBuffer.GetBufferView());
	std::vector<T> sortedDataGpu(count);
	sortBuffer.Download(sortedDataGpu.data(), count);

	for (int i = 0; i < count; i++)
		EXPECT_EQ(sortedDataCpu[i], sortedDataGpu[i]) << "Sort mismatch at " << i;

	// Sort with permutation:
	std::vector<size_t> permutationCpu(count);
	math::SortPermutation(uploadData, permutationCpu, compare);
	BufferTyped<T> permutationSortBuffer = BufferTyped<T>(count, "permutationSortBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> permutationBuffer = BufferTyped<uint32_t>(count, "permutationBuffer", BufferUsage::storage);
	permutationSortBuffer.Upload(uploadData.data(), count);
	GpuSort<T>::SortPermutationAndWait(permutationSortBuffer.GetBufferView(), permutationBuffer.GetBufferView());
	std::vector<T> permutationSortedDataGpu(count);
	std::vector<uint32_t> permutationGpu(count);
	permutationSortBuffer.Download(permutationSortedDataGpu.data(), count);
	permutationBuffer.Download(permutationGpu.data(), count);

	for (int i = 0; i < count; i++)
	{
		EXPECT_EQ(sortedDataCpu[i], permutationSortedDataGpu[i]) << "Permutation sort mismatch at " << i;
		EXPECT_EQ(permutationCpu[i], permutationGpu[i]) << "Permutation mismatch at " << i;
	}
}



TEST_F(TEST_GpuSort, LocalBitonicSort)
{
	// Number of elements:
	int count = 111;	// must be <= BLOCK_SIZE = 128 of localBitonicSort compute shader.

	// Prepare cpu array:
	std::vector<int> uploadData(count);
	for (int i = 0; i < count; i++)
		uploadData[i] = math::Random::Uniform(1, count);

	// Prepare gpu buffer:
	Buffer buffer = Buffer(count, sizeof(int), "dataBuffer", BufferUsage::storage);
	buffer.Upload(uploadData.data(), count * sizeof(int));

	// Prepare compute shader:
	std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
	ComputeShader sortCS = ComputeShader("localBitonicSort", directoryPath / "localBitonicSortInt.comp.spv");
	Uint3 threadCount(count / 2, 1, 1);
	uint32_t sessionID = Compute::Async::CreateComputeSession();
	ShaderProperties shaderProperties = Compute::Async::RecordComputeShader(sessionID, sortCS, threadCount);
	shaderProperties.SetBuffer("dataBuffer", buffer);
	shaderProperties.SetValue("Values", "bufferSize", count);

	// Sort array on cpu:
	std::vector<int> sortedDataCpu = math::CopySort(uploadData, [](int a, int b) { return a < b; });

	// Sort array on gpu:
	Compute::Async::DispatchComputeSessionAndWait(sessionID);

	// Buffer download:
	std::vector<int> sortedDataGpu(count);
	buffer.Download(sortedDataGpu.data(), count * sizeof(int));

	// Check if data is correct:
	bool allGood = true;
	for (int i = 0; i < count; i++)
	{
		if (sortedDataCpu[i] != sortedDataGpu[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Sort mismatch at " << i << ": cpu = " << sortedDataCpu[i] << ", gpu = " << sortedDataGpu[i];
		}
	}
	EXPECT_TRUE(allGood);
}



TEST_F(TEST_GpuSort, BitonicSortFloat2)
{
	std::vector<Float2> uploadData;
	for (int i = 0; i < 300; i++)
		uploadData.emplace_back(static_cast<float>(i % 20 - 10), static_cast<float>(i / 20 - 7));
	std::shuffle(uploadData.begin(), uploadData.end(), math::Random::GetEngine());

	TestBitonicSortAndPermutationSort(uploadData, [](const Float2& a, const Float2& b)
	{
		if (a.x != b.x)
			return a.x < b.x;
		return a.y < b.y;
	});
}



TEST_F(TEST_GpuSort, BitonicSortFloat3)
{
	std::vector<Float3> uploadData;
	for (int i = 0; i < 300; i++)
		uploadData.emplace_back(static_cast<float>(i % 6 - 3), static_cast<float>((i / 6) % 10 - 5), static_cast<float>(i / 60 - 2));
	std::shuffle(uploadData.begin(), uploadData.end(), math::Random::GetEngine());

	TestBitonicSortAndPermutationSort(uploadData, [](const Float3& a, const Float3& b)
	{
		if (a.x != b.x)
			return a.x < b.x;
		if (a.y != b.y)
			return a.y < b.y;
		return a.z < b.z;
	});
}



TEST_F(TEST_GpuSort, BitonicSortInt)
{
	// Number of elements:
	int count = 12345678;

	// Prepare cpu array:
	std::vector<int> dataCpu(count);
	for (int i = 0; i < count; i++)
		dataCpu[i] = math::Random::Uniform(1, count);

	// Prepare gpu buffer:
	BufferTyped<int> buffer = BufferTyped<int>(count, "buffer", BufferUsage::storage);
	buffer.Upload(dataCpu.data(), count);

	// Sort array on cpu:
	Time::Reset();
	math::Sort(dataCpu, [](int a, int b) { return a < b; });
	Time::Update();
	LOG_INFO("cpu sort time: {}s", Time::GetDeltaTime());

	// Sort array on gpu:
	GpuSort<int>::SortAndWait(buffer.GetBufferView());
	Time::Update();
	LOG_INFO("gpu sort time: {}s", Time::GetDeltaTime());

	// Download:
	std::vector<int> dataGpu(count);
	buffer.Download(dataGpu.data(), count);

	// Check results:
	bool allGood = true;
	for (int i = 0; i < count; i++)
	{
		if (dataCpu[i] != dataGpu[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Sort mismatch at " << i << ": cpu = " << dataCpu[i] << ", gpu = " << dataGpu[i];
		}
	}
	EXPECT_TRUE(allGood);
}



TEST_F(TEST_GpuSort, BitonicPermutationSortInt)
{
	// Number of elements:
	int count = 12345678;

	// Prepare cpu arrays:
	std::vector<int> dataCpu0(count);
	std::vector<int> dataCpu1(count);
	std::vector<size_t> permutationCpu(count);
	std::iota(dataCpu0.begin(), dataCpu0.end(), 0);
	std::iota(dataCpu1.begin(), dataCpu1.end(), 0);
	std::shuffle(dataCpu0.begin(), dataCpu0.end(), math::Random::GetEngine());
	std::shuffle(dataCpu1.begin(), dataCpu1.end(), math::Random::GetEngine());

	// Prepare gpu buffers:
	BufferTyped<int> dataBuffer0 = BufferTyped<int>(count, "dataBufferBuffer0", BufferUsage::storage);
	BufferTyped<int> dataBuffer1 = BufferTyped<int>(count, "dataBufferBuffer1", BufferUsage::storage);
	BufferTyped<int> tempBuffer = BufferTyped<int>(count, "tempBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> permutationBuffer = BufferTyped<uint32_t>(count, "permutationBuffer", BufferUsage::storage);
	dataBuffer0.Upload(dataCpu0.data(), count);
	dataBuffer1.Upload(dataCpu1.data(), count);

	// Sort array on cpu:
	Time::Reset();
	math::SortPermutation(dataCpu0, permutationCpu, [](int const& a, int const& b) { return a < b; });
	dataCpu0 = math::ApplyPermutation(dataCpu0, permutationCpu);
	dataCpu1 = math::ApplyPermutation(dataCpu1, permutationCpu);
	Time::Update();
	LOG_INFO("cpu sort time: {}s", Time::GetDeltaTime());

	// Sort array on gpu:
	GpuSort<int>::SortPermutationAndWait(dataBuffer0.GetBufferView(), permutationBuffer.GetBufferView());
	GpuSort<int>::ApplyPermutationAndWait(permutationBuffer.GetBufferView(), dataBuffer1.GetBufferView(), tempBuffer.GetBufferView());
	std::swap(dataBuffer1, tempBuffer);
	Time::Update();
	LOG_INFO("gpu sort time: {}s", Time::GetDeltaTime());

	// Download:
	std::vector<int> dataGpu0(count);
	std::vector<int> dataGpu1(count);
	std::vector<int> permutationGpu(count);
	dataBuffer0.Download(dataGpu0.data(), count);
	dataBuffer1.Download(dataGpu1.data(), count);
	permutationBuffer.Download(permutationGpu.data(), count);

	// Check results:
	bool allGood = true;
	for (int i = 0; i < count; i++)
	{
		if (permutationCpu[i] != permutationGpu[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Sort permutation mismatch, " << i << ": cpu = " << permutationCpu[i] << ", gpu = " << permutationGpu[i];
		}
		if (dataCpu0[i] != dataGpu0[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Sort mismatch at buffer0, " << i << ": cpu = " << dataCpu0[i] << ", gpu = " << dataGpu0[i];
		}
		if (dataCpu1[i] != dataGpu1[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Sort mismatch at buffer1, " << i << ": cpu = " << dataCpu1[i] << ", gpu = " << dataGpu1[i];
		}
	}

	// Inverse permutation Cpu:
	std::vector<size_t> inversePermutationCpu = math::InvertPermutation(permutationCpu);

	// Unsort on Cpu:
	dataCpu0 = math::ApplyPermutation(dataCpu0, inversePermutationCpu);
	dataCpu1 = math::ApplyPermutation(dataCpu1, inversePermutationCpu);

	// Inverse permutation Gpu:
	BufferTyped<uint32_t> inversePermutationBuffer = BufferTyped<uint32_t>(count, "inversePermutationBuffer", BufferUsage::storage);
	GpuSort<int>::InvertPermutationAndWait(permutationBuffer.GetBufferView(), inversePermutationBuffer.GetBufferView());
	inversePermutationBuffer.Download(permutationGpu.data(), count);

	// Unsort on Gpu:
	GpuSort<int>::ApplyPermutationAndWait(inversePermutationBuffer.GetBufferView(), dataBuffer0.GetBufferView(), tempBuffer.GetBufferView());
	std::swap(dataBuffer0, tempBuffer);
	dataBuffer0.Download(dataGpu0.data(), count);
	GpuSort<int>::ApplyPermutationAndWait(inversePermutationBuffer.GetBufferView(), dataBuffer1.GetBufferView(), tempBuffer.GetBufferView());
	std::swap(dataBuffer1, tempBuffer);
	dataBuffer1.Download(dataGpu1.data(), count);

	// Check results:
	for (int i = 0; i < count; i++)
	{
		if (inversePermutationCpu[i] != permutationGpu[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Inverse sort permutation mismatch, " << i << ": cpu = " << inversePermutationCpu[i] << ", gpu = " << permutationGpu[i];
		}
		if (dataCpu0[i] != dataGpu0[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Unsort mismatch at buffer0, " << i << ": cpu = " << dataCpu0[i] << ", gpu = " << dataGpu0[i];
		}
		if (dataCpu1[i] != dataGpu1[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Unsort mismatch at buffer0, " << i << ": cpu = " << dataCpu1[i] << ", gpu = " << dataGpu1[i];
		}
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
//		pCamera->SetProjectionType(Camera::ProjectionType::orthographic);
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