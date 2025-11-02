#include "sphBitonicSort3d.h"



namespace fluidDynamics
{
	// Initialization and cleanup:
	SphBitonicSort3d::SphBitonicSort3d()
	{
		std::string directoryPath = (std::string)PROJECT_ROOT_PATH + "/bin/shaders";
		m_cellKeys = ComputeShader("cellKeys3d", directoryPath + "/cellKeys3d.comp.spv");
		m_startIndices = ComputeShader("startIndices3d", directoryPath + "/startIndices3d.comp.spv");
		m_localBitonicSort = ComputeShader("sphLocalBitonicSort3d", directoryPath + "/sphLocalBitonicSort3d.comp.spv");
		m_bigFlip = ComputeShader("sphBigFlip3d", directoryPath + "/sphBigFlip3d.comp.spv");
		m_bigDisperse = ComputeShader("sphBigDisperse3d", directoryPath + "/sphBigDisperse3d.comp.spv");
		m_localDisperse = ComputeShader("sphLocalDisperse3d", directoryPath + "/sphLocalDisperse3d.comp.spv");
		m_cellKeyProperties = ShaderProperties(m_cellKeys);
		m_startIndicesProperties = ShaderProperties(m_startIndices);
	}
	SphBitonicSort3d::~SphBitonicSort3d()
	{

	}



	// Sorting:
	void SphBitonicSort3d::ComputeCellKeys(Buffer& cellKeyBuffer, Buffer& positionBuffer, float gridRadius)
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
	void SphBitonicSort3d::ComputeStartIndices(Buffer& cellKeyBuffer, Buffer& startIndexBuffer)
	{
		int particleCount = cellKeyBuffer.GetCount();
		Uint3 threadCount(particleCount, 1, 1);
		m_startIndicesProperties.SetBuffer("startIndexBuffer", startIndexBuffer);
		m_startIndicesProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer);
		Compute::PreRender::RecordComputeShader(m_startIndices, m_startIndicesProperties, threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphBitonicSort3d::Sort(Buffer& cellKeyBuffer, Buffer& positionBuffer, Buffer& velocityBuffer)
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