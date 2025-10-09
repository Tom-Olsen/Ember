#include "testInstancedRendering.h"



namespace emberEngine
{
	// Constructor/Destructor:
	TestInstancedRendering::TestInstancedRendering(uint32_t instanceCount)
	{
		uint32_t elementSize = sizeof(Float4x4) + sizeof(Float4);
		m_pInstanceBuffer = std::make_unique<Buffer>(instanceCount, elementSize, "testInstancedRendering", BufferUsage::storage);
		m_pStartCS = ComputeShaderManager::TryGetComputeShader("initialPositions");
		m_pUpdateCS = ComputeShaderManager::TryGetComputeShader("updatePositions");
		m_startProperties = ShaderProperties(*m_pStartCS);
		m_updateProperties = ShaderProperties(*m_pUpdateCS);
	}
	TestInstancedRendering::~TestInstancedRendering()
	{

	}



	// Public methods:
	Buffer* TestInstancedRendering::GetInstanceBuffer() const
	{
		return m_pInstanceBuffer.get();
	}



	// Overrides:
	void TestInstancedRendering::Start()
	{
		if (m_pStartCS != nullptr)
		{
			Uint3 threadCount = Uint3(m_pInstanceBuffer->GetCount(), 1, 1);
			m_startProperties.SetBuffer("instanceBuffer", *m_pInstanceBuffer);
			Compute::GetPreRender()->RecordComputeShader(*m_pStartCS, m_startProperties, threadCount);
			Compute::GetPreRender()->RecordBarrier(AccessMasks::computeShader_shaderWrite, AccessMasks::computeShader_shaderRead);
		}
	}
	void TestInstancedRendering::Update()
	{
		if (m_pUpdateCS != nullptr)
		{
			Uint3 threadCount = Uint3(m_pInstanceBuffer->GetCount(), 1, 1);
			m_updateProperties.SetBuffer("instanceBuffer", *m_pInstanceBuffer);
			Compute::GetPreRender()->RecordComputeShader(*m_pUpdateCS, m_updateProperties, threadCount);
		}
	}
	const std::string TestInstancedRendering::ToString() const
	{
		return "TestInstancedRendering";
	}
}