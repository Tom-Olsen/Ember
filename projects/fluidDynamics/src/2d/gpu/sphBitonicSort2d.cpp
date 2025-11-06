#include "sphBitonicSort2d.h"



namespace fluidDynamics
{
	// Initialization and cleanup:
	SphBitonicSort2d::SphBitonicSort2d()
	{
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		m_cellKeys = ComputeShader("cellKeys2d", directoryPath / "cellKeys2d.comp.spv");
		m_startIndices = ComputeShader("startIndices2d", directoryPath / "startIndices2d.comp.spv");
		m_localBitonicSort = ComputeShader("sphLocalBitonicSort2d", directoryPath / "sphLocalBitonicSort2d.comp.spv");
		m_bigFlip = ComputeShader("sphBigFlip2d", directoryPath / "sphBigFlip2d.comp.spv");
		m_bigDisperse = ComputeShader("sphBigDisperse2d", directoryPath / "sphBigDisperse2d.comp.spv");
		m_localDisperse = ComputeShader("sphLocalDisperse2d", directoryPath / "sphLocalDisperse2d.comp.spv");
		m_cellKeyProperties = ShaderProperties(m_cellKeys);
		m_startIndicesProperties = ShaderProperties(m_startIndices);
	}
	SphBitonicSort2d::~SphBitonicSort2d()
	{

	}



	// Sorting:
	void SphBitonicSort2d::ComputeCellKeys(Buffer& cellKeyBuffer, Buffer& positionBuffer, float gridRadius)
	{
		uint32_t particleCount = cellKeyBuffer.GetCount();
		Uint3 threadCount(particleCount, 1, 1);
		m_cellKeyProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer);
		m_cellKeyProperties.SetBuffer("positionBuffer", positionBuffer);
		m_cellKeyProperties.SetValue("Values", "gridRadius", gridRadius);
		m_cellKeyProperties.SetValue("Values", "particleCount", (int)particleCount);
		Compute::PreRender::RecordComputeShader(m_cellKeys, m_cellKeyProperties, threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphBitonicSort2d::ComputeStartIndices(Buffer& cellKeyBuffer, Buffer& startIndexBuffer)
	{
		int particleCount = cellKeyBuffer.GetCount();
		Uint3 threadCount(particleCount, 1, 1);
		m_startIndicesProperties.SetBuffer("startIndexBuffer", startIndexBuffer);
		m_startIndicesProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer);
		Compute::PreRender::RecordComputeShader(m_startIndices, m_startIndicesProperties, threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphBitonicSort2d::Sort(Buffer& cellKeyBuffer, Buffer& positionBuffer, Buffer& velocityBuffer)
	{
		int blockSize = m_localBitonicSort.GetBlockSize().x;
		int bufferSize = (int)positionBuffer.GetCount();			// total number of particles.
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.
		threadCountLocal = Uint3::Max(threadCountLocal, Uint3::one);
		threadCountBig = Uint3::Max(threadCountBig, Uint3::one);

		// Local bitonic sort for each block:
		ShaderProperties shaderProperties = Compute::PreRender::RecordComputeShader(m_localBitonicSort, threadCountLocal);
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer);
		shaderProperties.SetBuffer("positionBuffer", positionBuffer);
		shaderProperties.SetBuffer("velocityBuffer", velocityBuffer);
		shaderProperties.SetValue("Values", "bufferSize", bufferSize);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);

		for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
		{
			// Big flip:
			shaderProperties = Compute::PreRender::RecordComputeShader(m_bigFlip, threadCountBig);
			shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer);
			shaderProperties.SetBuffer("positionBuffer", positionBuffer);
			shaderProperties.SetBuffer("velocityBuffer", velocityBuffer);
			shaderProperties.SetValue("Values", "flipHeight", flipHeight);
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);

			for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
			{
				// Big disperse:
				shaderProperties = Compute::PreRender::RecordComputeShader(m_bigDisperse, threadCountBig);
				shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer);
				shaderProperties.SetBuffer("positionBuffer", positionBuffer);
				shaderProperties.SetBuffer("velocityBuffer", velocityBuffer);
				shaderProperties.SetValue("Values", "disperseHeight", disperseHeight);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
			}

			// Local disperse:
			shaderProperties = Compute::PreRender::RecordComputeShader(m_localDisperse, threadCountLocal);
			shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer);
			shaderProperties.SetBuffer("positionBuffer", positionBuffer);
			shaderProperties.SetBuffer("velocityBuffer", velocityBuffer);
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
		}
	}
}