#include "bitonicSort.h"
#include "accessMasks.h"
#include "compute.h"
#include "computeShader.h"
#include "logger.h"
#include "macros.h"
#include "shaderProperties.h"
#include "storageBuffer.h"



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

		std::string directoryPath = (std::string)CORE_SHADERS_DIR + "/bin";
		s_pLocalBitonicSort = std::make_unique<ComputeShader>("localBitonicSort", directoryPath + "/localBitonicSort.comp.spv");
		s_pBigFlip = std::make_unique<ComputeShader>("bigFlip", directoryPath + "/bigFlip.comp.spv");
		s_pBigDisperse = std::make_unique<ComputeShader>("bigDisperse", directoryPath + "/bigDisperse.comp.spv");
		s_pLocalDisperse = std::make_unique<ComputeShader>("localDisperse", directoryPath + "/localDisperse.comp.spv");

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("BitonicSort initialized.");
		#endif
	}
	void BitonicSort::Clear()
	{
		s_pLocalBitonicSort.reset();
		s_pBigFlip.reset();
		s_pBigDisperse.reset();
		s_pLocalDisperse.reset();
	}



	// Sorting:
	void BitonicSort::Sort(StorageBuffer* pBuffer)
	{
		ShaderProperties* pShaderProperties;
		int blockSize = s_pLocalBitonicSort->GetBlockSize().x;
		int bufferSize = (int)pBuffer->GetCount();					// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Create async compute session:
		uint32_t sessionID = compute::Async::CreateComputeSession();
		{
			// Local bitonic sort for each block:
			pShaderProperties = compute::Async::RecordComputeShader(sessionID, s_pLocalBitonicSort.get(), threadCountLocal);
			pShaderProperties->SetStorageBuffer("dataBuffer", pBuffer);
			pShaderProperties->SetValue("Values", "bufferSize", bufferSize);
			compute::Async::RecordBarrier(sessionID, AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
			
			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				pShaderProperties = compute::Async::RecordComputeShader(sessionID, s_pBigFlip.get(), threadCountBig);
				pShaderProperties->SetStorageBuffer("dataBuffer", pBuffer);
				pShaderProperties->SetValue("Values", "flipHeight", flipHeight);
				pShaderProperties->SetValue("Values", "bufferSize", bufferSize);
				compute::Async::RecordBarrier(sessionID, AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
				
				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					pShaderProperties = compute::Async::RecordComputeShader(sessionID, s_pBigDisperse.get(), threadCountBig);
					pShaderProperties->SetStorageBuffer("dataBuffer", pBuffer);
					pShaderProperties->SetValue("Values", "disperseHeight", disperseHeight);
					pShaderProperties->SetValue("Values", "bufferSize", bufferSize);
					compute::Async::RecordBarrier(sessionID, AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
				}
				
				// Local disperse:
				pShaderProperties = compute::Async::RecordComputeShader(sessionID, s_pLocalDisperse.get(), threadCountLocal);
				pShaderProperties->SetStorageBuffer("dataBuffer", pBuffer);
				pShaderProperties->SetValue("Values", "bufferSize", bufferSize);
				compute::Async::RecordBarrier(sessionID, AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
			}
		}
		// Dispatch and wait for async compute session:
		compute::Async::DispatchComputeSession(sessionID);
		compute::Async::WaitForFinish(sessionID);
	}
}