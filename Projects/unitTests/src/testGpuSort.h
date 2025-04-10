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
			settings.framesInFlight = 2;
			settings.msaaSamples = VK_SAMPLE_COUNT_4_BIT;
			settings.renderheight = 100;
			settings.renderWidth = 100;
			settings.vSyncEnabled = true;
			settings.windowHeight = 100;
			settings.windowWidth = 100;
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
		int count = 50;
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



	TEST_F(GpuSort, Sort)
	{
		// Create storage buffer:
		int count = 64;
		StorageBuffer buffer = StorageBuffer((uint32_t)count, (uint32_t)sizeof(int));

		// Create unsorted array:
		std::vector<int> unsortedData;
		unsortedData.resize(count);
		for (int i = 0; i < count; i++)
			unsortedData[i] = math::Random::Uniform(1, count);

		// Sort array on CPU:
		std::vector<int> sortedDataCpu = math::CopySort(unsortedData, [](int a, int b) { return a < b; });

		// Load compute shader and set shaderProperties:
		std::string directoryPath = (std::string)ENGINE_ROOT_PATH + "/src/shaders/bin";
		ComputeShader sortCS = ComputeShader("sort", directoryPath + "/sort.comp.spv");
		ShaderProperties shaderProperties = ShaderProperties((Shader*)&sortCS);
		shaderProperties.SetStorageBuffer("b_data", &buffer);
		
		// Dispatch compute shader:
		Uint3 threadCount(count, 1, 1);
		Compute::DispatchImmediatly(&sortCS, &shaderProperties, threadCount);

		// Download data from GPU:
		std::vector<int> sortedDataGpu;
		sortedDataGpu.resize(count);
		uint32_t size = count * sizeof(int);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.DownloadFromBuffer(buffer.GetVmaBuffer(), VulkanContext::pLogicalDevice->GetTransferQueue());
		stagingBuffer.GetData(sortedDataGpu.data(), size);

		// Print data:
		for (int i = 0; i < count; i++)
			LOG_TRACE("CPU:{}, GPU:{}", sortedDataCpu[i], sortedDataGpu[i]);

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
}



#endif // __INCLUDE_GUARD_testGpuSort_h__