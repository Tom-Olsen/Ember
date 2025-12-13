#include "application.h"
#include "emberEngine.h"
#include <gtest/gtest.h>



using namespace emberEngine;



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