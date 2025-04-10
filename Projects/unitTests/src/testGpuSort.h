#ifndef __INCLUDE_GUARD_testGpuSort_h__
#define __INCLUDE_GUARD_testGpuSort_h__
#include "application.h"
#include "compute.h"
#include "computeShader.h"
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
				return;
		}
		EXPECT_TRUE(allGood);
	}



	TEST_F(GpuSort, Sort)
	{
		// Create storage buffer:
		int count = 50;
		StorageBuffer buffer = StorageBuffer((uint32_t)count, (uint32_t)sizeof(float));

		// Load compute shader:
		std::string directoryPath = (std::string)ENGINE_ROOT_PATH + "/src/shaders/bin";
		ComputeShader sortCS = ComputeShader("sort", directoryPath + "/sort.comp.spv");

		// Perpare shader Properties:
		ShaderProperties shaderProperties = ShaderProperties((Shader*)&sortCS);
		shaderProperties.SetStorageBuffer("b_data", &buffer);
	
		// Dispatch compute shader:
		Uint3 threadCount(count, 1, 1);
		Compute::DispatchImmediatly(&sortCS, &shaderProperties, threadCount);

		// Create CPU pointer for the data:
		std::vector<float> data;
		data.resize(count);

		// Create Staging buffer:
		uint32_t size = count * sizeof(float);
		StagingBuffer stagingBuffer(size);

		// Download data from GPU:
		stagingBuffer.DownloadFromBuffer(buffer.GetVmaBuffer(), VulkanContext::pLogicalDevice->GetTransferQueue());
		stagingBuffer.GetData(data.data(), size);
		
		// Check if data is correct:
		bool allGood = true;
		for (int i = 0; i < count; i++)
		{
			allGood = (data[i] == i);
			if (allGood == false)
				return;
		}
		EXPECT_TRUE(allGood);
	}
}



#endif // __INCLUDE_GUARD_testGpuSort_h__