#include "sphBitonicSort2d.h"
#include "accessMasks.h"
#include "compute.h"
#include "computeShader.h"
#include "logger.h"
#include "shaderProperties.h"
#include "storageBuffer.h"



namespace emberEngine
{
	// Initialization and cleanup:
	SphBitonicSort2d::SphBitonicSort2d()
	{
		std::string directoryPath = (std::string)PROJECT_ROOT_PATH + "/src/shaders/bin";
		m_pCellKeys = std::make_unique<ComputeShader>("cellKeys2d", directoryPath + "/cellKeys2d.comp.spv");
		m_pStartIndices = std::make_unique<ComputeShader>("startIndices2d", directoryPath + "/startIndices2d.comp.spv");
		m_pLocalBitonicSort = std::make_unique<ComputeShader>("sphLocalBitonicSort2d", directoryPath + "/sphLocalBitonicSort2d.comp.spv");
		m_pBigFlip = std::make_unique<ComputeShader>("sphBigFlip2d", directoryPath + "/sphBigFlip2d.comp.spv");
		m_pBigDisperse = std::make_unique<ComputeShader>("sphBigDisperse2d", directoryPath + "/sphBigDisperse2d.comp.spv");
		m_pLocalDisperse = std::make_unique<ComputeShader>("sphLocalDisperse2d", directoryPath + "/sphLocalDisperse2d.comp.spv");
		m_pCellKeyProperties = std::make_unique<ShaderProperties>((Shader*)m_pCellKeys.get());
		m_pStartIndicesProperties = std::make_unique<ShaderProperties>((Shader*)m_pStartIndices.get());
	}
	SphBitonicSort2d::~SphBitonicSort2d()
	{
		m_pLocalBitonicSort.reset();
		m_pBigFlip.reset();
		m_pBigDisperse.reset();
		m_pLocalDisperse.reset();
	}



	// Sorting:
	void SphBitonicSort2d::ComputeCellKeys(StorageBuffer* pCellKeyBuffer, StorageBuffer* pPositionBuffer, float gridRadius)
	{
		uint32_t particleCount = pCellKeyBuffer->GetCount();
		Uint3 threadCount(particleCount, 1, 1);
		m_pCellKeyProperties->SetStorageBuffer("cellKeyBuffer", pCellKeyBuffer);
		m_pCellKeyProperties->SetStorageBuffer("positionBuffer", pPositionBuffer);
		m_pCellKeyProperties->SetValue("Values", "gridRadius", gridRadius);
		m_pCellKeyProperties->SetValue("Values", "particleCount", (int)particleCount);
		compute::PreRender::RecordComputeShader(m_pCellKeys.get(), m_pCellKeyProperties.get(), threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphBitonicSort2d::ComputeStartIndices(StorageBuffer* pCellKeyBuffer, StorageBuffer* pStartIndexBuffer)
	{
		int particleCount = pCellKeyBuffer->GetCount();
		Uint3 threadCount(particleCount, 1, 1);
		m_pStartIndicesProperties->SetStorageBuffer("startIndexBuffer", pStartIndexBuffer);
		m_pStartIndicesProperties->SetStorageBuffer("cellKeyBuffer", pCellKeyBuffer);
		compute::PreRender::RecordComputeShader(m_pStartIndices.get(), m_pStartIndicesProperties.get(), threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphBitonicSort2d::Sort(StorageBuffer* pCellKeyBuffer, StorageBuffer* pPositionBuffer, StorageBuffer* pVelocityBuffer)
	{
		ShaderProperties* pShaderProperties;
		int blockSize = m_pLocalBitonicSort->GetBlockSize().x;
		int bufferSize = (int)pPositionBuffer->GetCount();			// total number of particles.
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.
		threadCountLocal = Uint3::Max(threadCountLocal, Uint3::one);
		threadCountBig = Uint3::Max(threadCountBig, Uint3::one);

		// Local bitonic sort for each block:
		pShaderProperties = compute::PreRender::RecordComputeShader(m_pLocalBitonicSort.get(), threadCountLocal);
		pShaderProperties->SetStorageBuffer("cellKeyBuffer", pCellKeyBuffer);
		pShaderProperties->SetStorageBuffer("positionBuffer", pPositionBuffer);
		pShaderProperties->SetStorageBuffer("velocityBuffer", pVelocityBuffer);
		pShaderProperties->SetValue("Values", "bufferSize", bufferSize);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);

		for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
		{
			// Big flip:
			pShaderProperties = compute::PreRender::RecordComputeShader(m_pBigFlip.get(), threadCountBig);
			pShaderProperties->SetStorageBuffer("cellKeyBuffer", pCellKeyBuffer);
			pShaderProperties->SetStorageBuffer("positionBuffer", pPositionBuffer);
			pShaderProperties->SetStorageBuffer("velocityBuffer", pVelocityBuffer);
			pShaderProperties->SetValue("Values", "flipHeight", flipHeight);
			pShaderProperties->SetValue("Values", "bufferSize", bufferSize);
			compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);

			for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
			{
				// Big disperse:
				pShaderProperties = compute::PreRender::RecordComputeShader(m_pBigDisperse.get(), threadCountBig);
				pShaderProperties->SetStorageBuffer("cellKeyBuffer", pCellKeyBuffer);
				pShaderProperties->SetStorageBuffer("positionBuffer", pPositionBuffer);
				pShaderProperties->SetStorageBuffer("velocityBuffer", pVelocityBuffer);
				pShaderProperties->SetValue("Values", "disperseHeight", disperseHeight);
				pShaderProperties->SetValue("Values", "bufferSize", bufferSize);
				compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
			}

			// Local disperse:
			pShaderProperties = compute::PreRender::RecordComputeShader(m_pLocalDisperse.get(), threadCountLocal);
			pShaderProperties->SetStorageBuffer("cellKeyBuffer", pCellKeyBuffer);
			pShaderProperties->SetStorageBuffer("positionBuffer", pPositionBuffer);
			pShaderProperties->SetStorageBuffer("velocityBuffer", pVelocityBuffer);
			pShaderProperties->SetValue("Values", "bufferSize", bufferSize);
			compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
		}
	}
}