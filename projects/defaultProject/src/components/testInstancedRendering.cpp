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
			ShaderProperties shaderProperties = Compute::PreRender::RecordComputeShader(*m_pStartCS, threadCount);
			shaderProperties.SetBuffer("instanceBuffer", *m_pInstanceBuffer);
			Compute::PreRender::RecordBarrierWaitShaderWriteBeforeRead();
		}
	}
	void TestInstancedRendering::Update()
	{
		if (m_pUpdateCS != nullptr)
		{
			Uint3 threadCount = Uint3(m_pInstanceBuffer->GetCount(), 1, 1);
			ShaderProperties shaderProperties = Compute::PreRender::RecordComputeShader(*m_pUpdateCS, threadCount);
			shaderProperties.SetBuffer("instanceBuffer", *m_pInstanceBuffer);
		}
	}
}