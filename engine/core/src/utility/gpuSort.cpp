#include "gpuSort.h"



namespace emberEngine
{
	// Static members:
	bool GpuSort::s_isInitialized = false;
	// Bitonic sort compute shaders:
	std::unique_ptr<ComputeShader> GpuSort::s_pLocalBitonicSortIntComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pBigFlipIntComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pBigDisperseIntComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pLocalDisperseIntComputeShader;
	// Apply permutation compute shaders:
	std::unique_ptr<ComputeShader> GpuSort::s_pApplyPermutationFloat1ComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pApplyPermutationFloat2ComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pApplyPermutationFloat3ComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pApplyPermutationFloat4ComputeShader;
	// Apply permutation shader properties:
	std::unique_ptr<ShaderProperties> GpuSort::s_pApplyPermutationFloat1ShaderProperties;
	std::unique_ptr<ShaderProperties> GpuSort::s_pApplyPermutationFloat2ShaderProperties;
	std::unique_ptr<ShaderProperties> GpuSort::s_pApplyPermutationFloat3ShaderProperties;
	std::unique_ptr<ShaderProperties> GpuSort::s_pApplyPermutationFloat4ShaderProperties;



	// Public methods:
	// Initialization/Cleanup:
	void GpuSort::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();

		s_pLocalBitonicSortIntComputeShader = std::make_unique<ComputeShader>("localBitonicSortInt", directoryPath / "localBitonicSortInt.comp.spv");
		s_pBigFlipIntComputeShader = std::make_unique<ComputeShader>("bigFlipInt", directoryPath / "bigFlipInt.comp.spv");
		s_pBigDisperseIntComputeShader = std::make_unique<ComputeShader>("bigDisperseInt", directoryPath / "bigDisperseInt.comp.spv");
		s_pLocalDisperseIntComputeShader = std::make_unique<ComputeShader>("localDisperseInt", directoryPath / "localDisperseInt.comp.spv");

		s_pApplyPermutationFloat1ComputeShader = std::make_unique<ComputeShader>("applyPermutationFloat1", directoryPath / "applyPermutationFloat1.comp.spv");
		s_pApplyPermutationFloat2ComputeShader = std::make_unique<ComputeShader>("applyPermutationFloat2", directoryPath / "applyPermutationFloat2.comp.spv");
		s_pApplyPermutationFloat3ComputeShader = std::make_unique<ComputeShader>("applyPermutationFloat3", directoryPath / "applyPermutationFloat3.comp.spv");
		s_pApplyPermutationFloat4ComputeShader = std::make_unique<ComputeShader>("applyPermutationFloat4", directoryPath / "applyPermutationFloat4.comp.spv");
		s_pApplyPermutationFloat1ShaderProperties = std::make_unique<ShaderProperties>(*s_pApplyPermutationFloat1ComputeShader);
		s_pApplyPermutationFloat2ShaderProperties = std::make_unique<ShaderProperties>(*s_pApplyPermutationFloat2ComputeShader);
		s_pApplyPermutationFloat3ShaderProperties = std::make_unique<ShaderProperties>(*s_pApplyPermutationFloat3ComputeShader);
		s_pApplyPermutationFloat4ShaderProperties = std::make_unique<ShaderProperties>(*s_pApplyPermutationFloat4ComputeShader);
	}
	void GpuSort::Clear()
	{
		s_pLocalBitonicSortIntComputeShader.reset();
		s_pBigFlipIntComputeShader.reset();
		s_pBigDisperseIntComputeShader.reset();
		s_pLocalDisperseIntComputeShader.reset();
		s_pApplyPermutationFloat1ComputeShader.reset();
		s_pApplyPermutationFloat2ComputeShader.reset();
		s_pApplyPermutationFloat3ComputeShader.reset();
		s_pApplyPermutationFloat4ComputeShader.reset();
		s_pApplyPermutationFloat1ShaderProperties.reset();
		s_pApplyPermutationFloat2ShaderProperties.reset();
		s_pApplyPermutationFloat3ShaderProperties.reset();
		s_pApplyPermutationFloat4ShaderProperties.reset();
		s_isInitialized = false;
	}



	// Sort:
	void GpuSort::Sort(ComputeType computeType, Buffer& buffer)
	{
		// Sorting makes no sense for PostRender compute, as it is only for post processing effects:
		assert(computeType != ComputeType::postRender);

		// Gpu buffer access setup:
		int blockSize = s_pLocalBitonicSortIntComputeShader->GetBlockSize().x;
		int bufferSize = static_cast<int>(buffer.GetCount());		// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Sorting requires multiple synced kernel dispatches. Immediate dispatch mode gets emmulated by an async launch with wait for completion:
		uint32_t sessionID = -1;
		bool isImmediateCompute = computeType == ComputeType::immediate;
		if (isImmediateCompute)
		{
			sessionID = Compute::Async::CreateComputeSession();
			computeType = ComputeType::async;
		}
		{
			// Local bitonic sort for each block:
			ShaderProperties shaderProperties = Compute::RecordComputeShader(computeType, *s_pLocalBitonicSortIntComputeShader, threadCountLocal, sessionID);
			shaderProperties.SetBuffer("dataBuffer", buffer);
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
			
			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				shaderProperties = Compute::RecordComputeShader(computeType, *s_pBigFlipIntComputeShader, threadCountBig, sessionID);
				shaderProperties.SetBuffer("dataBuffer", buffer);
				shaderProperties.SetValue("Values", "flipHeight", flipHeight);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
				
				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					shaderProperties = Compute::RecordComputeShader(omputeType, *s_pBigDisperseIntComputeShader, threadCountBig, sessionID);
					shaderProperties.SetBuffer("dataBuffer", buffer);
					shaderProperties.SetValue("Values", "disperseHeight", disperseHeight);
					shaderProperties.SetValue("Values", "bufferSize", bufferSize);
					Compute::RecordBarrierWaitStorageWriteBeforeRead(omputeType, sessionID);
				}
				
				// Local disperse:
				shaderProperties = Compute::RecordComputeShader(omputeType, *s_pLocalDisperseIntComputeShader, threadCountLocal, sessionID);
				shaderProperties.SetBuffer("dataBuffer", buffer);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(omputeType, sessionID);
			}
		}

		// For immediate compute we dispatch and wait for the async compute session:
		if (isImmediateCompute)
		{
			Compute::Async::DispatchComputeSession(sessionID);
			Compute::Async::WaitForFinish(sessionID);
		}
	}



	// Apply Permutation:
	void GpuSort::ApplyPermutationFloat(Buffer& permutationBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pApplyPermutationFloat1ShaderProperties->SetBuffer("permutationBuffer", permutationBuffer);
		s_pApplyPermutationFloat1ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pApplyPermutationFloat1ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Immediate::Dispatch(*s_pApplyPermutationFloat1ComputeShader, *s_pApplyPermutationFloat1ShaderProperties, threadCount);
	}
}