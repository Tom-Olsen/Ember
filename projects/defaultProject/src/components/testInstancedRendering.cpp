#include "testInstancedRendering.h"
using namespace emberCore;
using namespace emberEcs;



namespace emberEngine
{
	// Constructor/Destructor:
	TestInstancedRendering::TestInstancedRendering(uint32_t instanceCount)
	{
		uint32_t elementSize = sizeof(Float4x4) + sizeof(Float4);
		m_pInstanceBuffer = std::make_unique<Buffer>(instanceCount, elementSize, "testInstancedRendering", BufferUsage::storage);
		m_startComputeShader = ComputeShaderManager::TryGetComputeShader("initialPositions");
		m_updateComputeShader = ComputeShaderManager::TryGetComputeShader("updatePositions");
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
		if (m_startComputeShader.IsValid())
		{
			Uint3 threadCount = Uint3(m_pInstanceBuffer->GetCount(), 1, 1);
			CallProperties callProperties = Compute::PreRender::RecordComputeShader(m_startComputeShader, threadCount);
			callProperties.SetBuffer("instanceBuffer", *m_pInstanceBuffer);
			Compute::PreRender::RecordBarrierWaitShaderWriteBeforeRead();
		}
	}
	void TestInstancedRendering::Update()
	{
		if (m_updateComputeShader.IsValid())
		{
			Uint3 threadCount = Uint3(m_pInstanceBuffer->GetCount(), 1, 1);
			CallProperties callProperties = Compute::PreRender::RecordComputeShader(m_updateComputeShader, threadCount);
			callProperties.SetBuffer("instanceBuffer", *m_pInstanceBuffer);
		}
	}
}