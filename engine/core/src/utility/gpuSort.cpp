#include "gpuSort.h"



namespace emberEngine
{
	// Static members:
	bool GpuSort::s_isInitialized = false;
	std::unique_ptr<ComputeShader> GpuSort::s_pLocalBitonicSortComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pBigFlipComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pBigDisperseComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pLocalDisperseComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pReorderFloat1ComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pReorderFloat2ComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pReorderFloat3ComputeShader;
	std::unique_ptr<ComputeShader> GpuSort::s_pReorderFloat4ComputeShader;
	std::unique_ptr<ShaderProperties> GpuSort::s_pReorderFloat1ShaderProperties;
	std::unique_ptr<ShaderProperties> GpuSort::s_pReorderFloat2ShaderProperties;
	std::unique_ptr<ShaderProperties> GpuSort::s_pReorderFloat3ShaderProperties;
	std::unique_ptr<ShaderProperties> GpuSort::s_pReorderFloat4ShaderProperties;



	// Public methods:
	// Initialization/Cleanup:
	void GpuSort::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();

		s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSort", directoryPath / "localBitonicSort.comp.spv");
		s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlip", directoryPath / "bigFlip.comp.spv");
		s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperse", directoryPath / "bigDisperse.comp.spv");
		s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperse", directoryPath / "localDisperse.comp.spv");

		s_pReorderFloat1ComputeShader = std::make_unique<ComputeShader>("reorderFloat1", directoryPath / "reorderFloat1.comp.spv");
		s_pReorderFloat2ComputeShader = std::make_unique<ComputeShader>("reorderFloat2", directoryPath / "reorderFloat2.comp.spv");
		s_pReorderFloat3ComputeShader = std::make_unique<ComputeShader>("reorderFloat3", directoryPath / "reorderFloat3.comp.spv");
		s_pReorderFloat4ComputeShader = std::make_unique<ComputeShader>("reorderFloat4", directoryPath / "reorderFloat4.comp.spv");
		s_pReorderFloat1ShaderProperties = std::make_unique<ShaderProperties>(*s_pReorderFloat1ComputeShader);
		s_pReorderFloat2ShaderProperties = std::make_unique<ShaderProperties>(*s_pReorderFloat2ComputeShader);
		s_pReorderFloat3ShaderProperties = std::make_unique<ShaderProperties>(*s_pReorderFloat3ComputeShader);
		s_pReorderFloat4ShaderProperties = std::make_unique<ShaderProperties>(*s_pReorderFloat4ComputeShader);
	}
	void GpuSort::Clear()
	{
		s_pLocalBitonicSortComputeShader.reset();
		s_pBigFlipComputeShader.reset();
		s_pBigDisperseComputeShader.reset();
		s_pLocalDisperseComputeShader.reset();
		s_pReorderFloat1ComputeShader.reset();
		s_pReorderFloat2ComputeShader.reset();
		s_pReorderFloat3ComputeShader.reset();
		s_pReorderFloat4ComputeShader.reset();
		s_pReorderFloat1ShaderProperties.reset();
		s_pReorderFloat2ShaderProperties.reset();
		s_pReorderFloat3ShaderProperties.reset();
		s_pReorderFloat4ShaderProperties.reset();
		s_isInitialized = false;
	}



	// Sorting:
	void GpuSort::SortImmediate(Buffer& buffer)
	{
		int blockSize = s_pLocalBitonicSortComputeShader->GetBlockSize().x;
		int bufferSize = static_cast<int>(buffer.GetCount());		// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Create async compute session:
		uint32_t sessionID = Compute::Async::CreateComputeSession();
		{
			// Local bitonic sort for each block:
			ShaderProperties shaderProperties = Compute::Async::RecordComputeShader(sessionID, *s_pLocalBitonicSortComputeShader, threadCountLocal);
			shaderProperties.SetBuffer("dataBuffer", buffer);
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);
			
			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				shaderProperties = Compute::Async::RecordComputeShader(sessionID, *s_pBigFlipComputeShader, threadCountBig);
				shaderProperties.SetBuffer("dataBuffer", buffer);
				shaderProperties.SetValue("Values", "flipHeight", flipHeight);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);
				
				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					shaderProperties = Compute::Async::RecordComputeShader(sessionID, *s_pBigDisperseComputeShader, threadCountBig);
					shaderProperties.SetBuffer("dataBuffer", buffer);
					shaderProperties.SetValue("Values", "disperseHeight", disperseHeight);
					shaderProperties.SetValue("Values", "bufferSize", bufferSize);
					Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);
				}
				
				// Local disperse:
				shaderProperties = Compute::Async::RecordComputeShader(sessionID, *s_pLocalDisperseComputeShader, threadCountLocal);
				shaderProperties.SetBuffer("dataBuffer", buffer);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);
			}
		}

		// Dispatch and wait for async compute session:
		Compute::Async::DispatchComputeSession(sessionID);
		Compute::Async::WaitForFinish(sessionID);
	}
	void GpuSort::SortAsync(uint32_t sessionID, Buffer& buffer)
	{
		int blockSize = s_pLocalBitonicSortComputeShader->GetBlockSize().x;
		int bufferSize = static_cast<int>(buffer.GetCount());		// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Local bitonic sort for each block:
		ShaderProperties shaderProperties = Compute::Async::RecordComputeShader(sessionID, *s_pLocalBitonicSortComputeShader, threadCountLocal);
		shaderProperties.SetBuffer("dataBuffer", buffer);
		shaderProperties.SetValue("Values", "bufferSize", bufferSize);
		Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);

		for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
		{
			// Big flip:
			shaderProperties = Compute::Async::RecordComputeShader(sessionID, *s_pBigFlipComputeShader, threadCountBig);
			shaderProperties.SetBuffer("dataBuffer", buffer);
			shaderProperties.SetValue("Values", "flipHeight", flipHeight);
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);

			for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
			{
				// Big disperse:
				shaderProperties = Compute::Async::RecordComputeShader(sessionID, *s_pBigDisperseComputeShader, threadCountBig);
				shaderProperties.SetBuffer("dataBuffer", buffer);
				shaderProperties.SetValue("Values", "disperseHeight", disperseHeight);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);
			}

			// Local disperse:
			shaderProperties = Compute::Async::RecordComputeShader(sessionID, *s_pLocalDisperseComputeShader, threadCountLocal);
			shaderProperties.SetBuffer("dataBuffer", buffer);
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::Async::RecordBarrierWaitStorageWriteBeforeRead(sessionID);
		}
	}
	void GpuSort::SortPreRender(Buffer& buffer)
	{
		int blockSize = s_pLocalBitonicSortComputeShader->GetBlockSize().x;
		int bufferSize = static_cast<int>(buffer.GetCount());		// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Local bitonic sort for each block:
		ShaderProperties shaderProperties = Compute::PreRender::RecordComputeShader(*s_pLocalBitonicSortComputeShader, threadCountLocal);
		shaderProperties.SetBuffer("dataBuffer", buffer);
		shaderProperties.SetValue("Values", "bufferSize", bufferSize);
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

		for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
		{
			// Big flip:
			shaderProperties = Compute::PreRender::RecordComputeShader(*s_pBigFlipComputeShader, threadCountBig);
			shaderProperties.SetBuffer("dataBuffer", buffer);
			shaderProperties.SetValue("Values", "flipHeight", flipHeight);
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
			{
				// Big disperse:
				shaderProperties = Compute::PreRender::RecordComputeShader(*s_pBigDisperseComputeShader, threadCountBig);
				shaderProperties.SetBuffer("dataBuffer", buffer);
				shaderProperties.SetValue("Values", "disperseHeight", disperseHeight);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
			}

			// Local disperse:
			shaderProperties = Compute::PreRender::RecordComputeShader(*s_pLocalDisperseComputeShader, threadCountLocal);
			shaderProperties.SetBuffer("dataBuffer", buffer);
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		}
	}



	// Reorder immediate:
	void GpuSort::ReorderFloatImmediate(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat1ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat1ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat1ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Immediate::Dispatch(*s_pReorderFloat1ComputeShader, *s_pReorderFloat1ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat2Immediate(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat2ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat2ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat2ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Immediate::Dispatch(*s_pReorderFloat2ComputeShader, *s_pReorderFloat2ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat3Immediate(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat3ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat3ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat3ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Immediate::Dispatch(*s_pReorderFloat3ComputeShader, *s_pReorderFloat3ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat4Immediate(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat4ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat4ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat4ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Immediate::Dispatch(*s_pReorderFloat4ComputeShader, *s_pReorderFloat4ShaderProperties, threadCount);
	}



	// Reorder async:
	void GpuSort::ReorderFloatAsync(uint32_t sessionID, Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat1ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat1ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat1ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Async::RecordComputeShader(sessionID, *s_pReorderFloat1ComputeShader, *s_pReorderFloat1ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat2Async(uint32_t sessionID, Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat2ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat2ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat2ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Async::RecordComputeShader(sessionID, *s_pReorderFloat2ComputeShader, *s_pReorderFloat2ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat3Async(uint32_t sessionID, Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat3ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat3ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat3ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Async::RecordComputeShader(sessionID, *s_pReorderFloat3ComputeShader, *s_pReorderFloat3ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat4Async(uint32_t sessionID, Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat4ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat4ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat4ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::Async::RecordComputeShader(sessionID, *s_pReorderFloat4ComputeShader, *s_pReorderFloat4ShaderProperties, threadCount);
	}
	
	
	
	// Reorder pre render:
	void GpuSort::ReorderFloatPreRender(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat1ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat1ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat1ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::PreRender::RecordComputeShader(*s_pReorderFloat1ComputeShader, *s_pReorderFloat1ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat2PreRender(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat2ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat2ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat2ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::PreRender::RecordComputeShader(*s_pReorderFloat2ComputeShader, *s_pReorderFloat2ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat3PreRender(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat3ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat3ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat3ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::PreRender::RecordComputeShader(*s_pReorderFloat3ComputeShader, *s_pReorderFloat3ShaderProperties, threadCount);
	}
	void GpuSort::ReorderFloat4PreRender(Buffer& indexBuffer, Buffer& inBuffer, Buffer& outBuffer)
	{
		Uint3 threadCount = Uint3(indexBuffer.GetCount(), 1, 1);
		s_pReorderFloat4ShaderProperties->SetBuffer("indexBuffer", indexBuffer);
		s_pReorderFloat4ShaderProperties->SetBuffer("inBuffer", inBuffer);
		s_pReorderFloat4ShaderProperties->SetBuffer("outBuffer", outBuffer);
		Compute::PreRender::RecordComputeShader(*s_pReorderFloat4ComputeShader, *s_pReorderFloat4ShaderProperties, threadCount);
	}
}