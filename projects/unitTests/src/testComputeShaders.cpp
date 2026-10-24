#include "application.h"
#include "emberEngine.h"
#include "vulkanAsyncCompute.h"
#include <array>
#include <gtest/gtest.h>
#include <set>
#include <vector>



using namespace emberCore;
using namespace emberEcs;



// This setUp/tearDown runs once for all test belonging to this suite/fixture:
class TEST_ComputeShaders : public ::testing::Test
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



TEST_F(TEST_ComputeShaders, UploadDownload)
{
	// Number of elements:
	int count = 1234;

	// Prepare cpu arrays:
	std::vector<float> uploadData(count);
	std::vector<float> downloadData(count);
	for (int i = 0; i < count; i++)
		uploadData[i] = (float)i;

	// Buffer upload and download:
	Buffer buffer = Buffer(count, sizeof(float), "buffer", BufferUsage::storage);
	buffer.Upload(uploadData.data(), count * sizeof(float));
	buffer.Download(downloadData.data(), count * sizeof(float));

	// Check if data is correct:
	bool allGood = true;
	for (int i = 0; i < count; i++)
	{
		if (uploadData[i] != downloadData[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Data mismatch at " << i << ": uploadData = " << uploadData[i] << ", downloadData = " << downloadData[i];
		}
		if (allGood == false)
			break;
	}
	EXPECT_TRUE(allGood);
}



TEST_F(TEST_ComputeShaders, BufferSwap)
{
	// Number of elements:
	int count = 1234;

	// Prepare cpu arrays:
	std::vector<float> uploadData(count);
	std::vector<float> downloadData(count);
	for (int i = 0; i < count; i++)
		uploadData[i] = (float)i;

	// Buffer upload and download:
	Buffer bufferA = Buffer(count, sizeof(float), "bufferA", BufferUsage::storage);
	bufferA.Upload(uploadData.data(), count * sizeof(float));
	Buffer bufferB = Buffer(count, sizeof(float), "bufferB", BufferUsage::storage);
	std::swap(bufferA, bufferB);
	bufferB.Download(downloadData.data(), count * sizeof(float));

	// Check if data is correct:
	bool allGood = true;
	for (int i = 0; i < count; i++)
	{
		if (uploadData[i] != downloadData[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Data mismatch at " << i << ": uploadData = " << uploadData[i] << ", downloadData = " << downloadData[i];
		}
		if (allGood == false)
			break;
	}
	EXPECT_TRUE(allGood);
}



TEST_F(TEST_ComputeShaders, BufferViewSwap)
{
	// Number of elements:
	int count = 1234;

	// Prepare cpu arrays:
	std::vector<float> uploadData(count);
	std::vector<float> downloadData(count);
	for (int i = 0; i < count; i++)
		uploadData[i] = (float)i;

	// Buffer upload and download:
	Buffer bufferA = Buffer(count, sizeof(float), "bufferA", BufferUsage::storage);
	BufferView<float> bufferViewA(bufferA);
	bufferViewA.Upload(uploadData.data(), count);
	Buffer bufferB = Buffer(count, sizeof(float), "bufferB", BufferUsage::storage);
	BufferView<float> bufferViewB(bufferB);
	std::swap(bufferViewA, bufferViewB);
	bufferViewB.Download(downloadData.data(), count);
	
	// Check if data is correct:
	bool allGood = true;
	for (int i = 0; i < count; i++)
	{
		if (uploadData[i] != downloadData[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Data mismatch at " << i << ": uploadData = " << uploadData[i] << ", downloadData = " << downloadData[i];
		}
		if (allGood == false)
			break;
	}
	EXPECT_TRUE(allGood);
}



TEST_F(TEST_ComputeShaders, BufferTypedSwap)
{
	// Number of elements:
	int count = 1234;

	// Prepare cpu arrays:
	std::vector<float> uploadData(count);
	std::vector<float> downloadData(count);
	for (int i = 0; i < count; i++)
		uploadData[i] = (float)i;

	// Buffer upload and download:
	BufferTyped<float> bufferA = BufferTyped<float>(count, "bufferA", BufferUsage::storage);
	bufferA.Upload(uploadData.data(), count);
	BufferTyped<float> bufferB = BufferTyped<float>(count, "bufferB", BufferUsage::storage);
	std::swap(bufferA, bufferB);
	bufferB.Download(downloadData.data(), count);

	// Check if data is correct:
	bool allGood = true;
	for (int i = 0; i < count; i++)
	{
		if (uploadData[i] != downloadData[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Data mismatch at " << i << ": uploadData = " << uploadData[i] << ", downloadData = " << downloadData[i];
		}
		if (allGood == false)
			break;
	}
	EXPECT_TRUE(allGood);
}



TEST_F(TEST_ComputeShaders, AsyncSessionsGrowAndReuse)
{
	constexpr uint32_t sessionCount = 33;
	vulkanRendererBackend::Async async;
	for (uint32_t batchIndex = 0; batchIndex < 3; batchIndex++)
	{
		std::array<uint32_t, sessionCount> sessions;
		// Keep every slot recording so completed GPU work cannot hide the need to grow.
		for (uint32_t index = 0; index < sessionCount; index++)
		{
			sessions[index] = async.CreateComputeSession();
			EXPECT_LT(sessions[index], sessionCount);
		}
		std::set<uint32_t> uniqueSessions(sessions.begin(), sessions.end());
		EXPECT_EQ(uniqueSessions.size(), sessionCount);

		for (uint32_t sessionID : sessions)
			async.DispatchComputeSession(sessionID, 0.0f, 0.0f);
		for (auto session = sessions.rbegin(); session != sessions.rend(); session++)
			async.WaitForFinish(*session);
		for (uint32_t sessionID : sessions)
			EXPECT_TRUE(async.IsFinished(sessionID));
	}
}



TEST_F(TEST_ComputeShaders, AsyncSessionGrowthPreservesRecordedWork)
{
	constexpr uint32_t sessionCount = 33;
	constexpr uint32_t elementCount = 257;
	ComputeShader shader = ComputeShaderManager::TryGetComputeShader("initIndexBuffer");
	std::vector<BufferTyped<uint32_t>> buffers;
	buffers.reserve(sessionCount);
	for (uint32_t index = 0; index < sessionCount; index++)
		buffers.emplace_back(elementCount, "growingAsyncBuffer" + std::to_string(index), BufferUsage::storage);

	for (uint32_t batchIndex = 0; batchIndex < 2; batchIndex++)
	{
		std::array<uint32_t, sessionCount> sessions;
		std::vector<uint32_t> initialValues(elementCount, UINT32_MAX);
		for (uint32_t index = 0; index < sessionCount; index++)
		{
			buffers[index].Upload(initialValues.data(), elementCount);
			sessions[index] = Compute::Async::CreateComputeSession();
			CallProperties properties = Compute::Async::RecordComputeShader(sessions[index], shader, Uint3(elementCount, 1, 1));
			properties.SetBuffer("indexBuffer", buffers[index].GetBuffer());
		}

		// Growth must preserve the calls and bindings recorded in earlier sessions.
		for (uint32_t sessionID : sessions)
			Compute::Async::DispatchComputeSession(sessionID);
		for (auto session = sessions.rbegin(); session != sessions.rend(); session++)
			Compute::Async::WaitForFinish(*session);

		for (uint32_t index = 0; index < sessionCount; index++)
		{
			std::vector<uint32_t> result(elementCount);
			buffers[index].Download(result.data(), elementCount);
			for (uint32_t elementIndex = 0; elementIndex < elementCount; elementIndex++)
				EXPECT_EQ(result[elementIndex], elementIndex) << "Batch " << batchIndex << ", session " << index;
		}
	}
}