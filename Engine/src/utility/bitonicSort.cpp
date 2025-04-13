#include "bitonicSort.h"
#include "compute.h"
#include "computeShader.h"
#include "logger.h"
#include "shaderProperties.h"
#include "vulkanUtility.h"



namespace emberEngine
{
	// Static members:
	bool BitonicSort::s_isInitialized = false;
	std::unique_ptr<ComputeShader> BitonicSort::s_pLocalBitonicSort;
	std::unique_ptr<ComputeShader> BitonicSort::s_pBigFlip;
	std::unique_ptr<ComputeShader> BitonicSort::s_pBigDisperse;
	std::unique_ptr<ComputeShader> BitonicSort::s_pLocalDisperse;



	// Initialization and cleanup:
	void BitonicSort::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		std::string directoryPath = (std::string)ENGINE_ROOT_PATH + "/src/shaders/bin";
		s_pLocalBitonicSort = std::make_unique<ComputeShader>("localBitonicSort", directoryPath + "/localBitonicSort.comp.spv");
		s_pBigFlip = std::make_unique<ComputeShader>("bigFlip", directoryPath + "/bigFlip.comp.spv");
		s_pBigDisperse = std::make_unique<ComputeShader>("bigDisperse", directoryPath + "/bigDisperse.comp.spv");
		s_pLocalDisperse = std::make_unique<ComputeShader>("localDisperse", directoryPath + "/localDisperse.comp.spv");
	}
	void BitonicSort::Clear()
	{
		s_pLocalBitonicSort.release();
		s_pBigFlip.release();
		s_pBigDisperse.release();
		s_pLocalDisperse.release();
	}



	// Sorting:
	void BitonicSort::Sort(StorageBuffer* pBuffer, int count)
	{
		int localCount = 16;
		int localBatches = (count - 1) / localCount + 1;
		int remaining = count;
		int bufferSize = localCount;

		Compute::BeginAsyncCompute();
		// Dispatch localBitonicSort once for each local batch:
		for (int i = 0; i < localBatches; i++)
		{
			if (remaining < localCount)
				bufferSize = remaining;
			else
				remaining -= localCount;

			int offset = i * localCount;
			Uint3 threadCount(bufferSize / 2, 1, 1);
			ShaderProperties* shaderProperties = Compute::DispatchAsync(s_pLocalBitonicSort.get(), threadCount);
			shaderProperties->SetStorageBuffer("sortBuffer", pBuffer);
			shaderProperties->SetValue("Values", "offset", offset);
			shaderProperties->SetValue("Values", "bufferSize", bufferSize);
		}
		// Memory barrier:
		Compute::BarrierAsync(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
		// Big flip:

		Compute::EndAsyncCompute();
	}
}