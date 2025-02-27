#include "testInstancedRendering.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Constructor/Destructor:
	TestInstancedRendering::TestInstancedRendering(VulkanContext* pVulkanContext, uint32_t instanceCount)
	{
		m_pVulkanContext = pVulkanContext;
		m_pStorageBuffer = std::make_unique<StorageBuffer>(m_pVulkanContext, instanceCount, sizeof(Float4x4));
		m_pStartCS = ComputeShaderManager::GetComputeShader("initialPositionsComputeShader");
		m_pUpdateCS = ComputeShaderManager::GetComputeShader("updatePositionsComputeShader");
	}
	TestInstancedRendering::~TestInstancedRendering()
	{

	}



	// Public methods:
	StorageBuffer* TestInstancedRendering::GetInstanceBuffer() const
	{
		return m_pStorageBuffer.get();
	}



	// Overrides:
	void TestInstancedRendering::Start()
	{
		if (m_pStartCS != nullptr)
		{
			Uint3 threadCount = Uint3(m_pStorageBuffer->GetCount(), 1, 1);
			ShaderProperties* shaderProperties = Compute::Dispatch(m_pStartCS, threadCount);
			shaderProperties->SetStorageBuffer("instanceBuffer", m_pStorageBuffer.get());
		}
	}
	void TestInstancedRendering::Update()
	{
		if (m_pUpdateCS != nullptr)
		{
			Uint3 threadCount = Uint3(m_pStorageBuffer->GetCount(), 1, 1);
			ShaderProperties* shaderProperties = Compute::Dispatch(m_pUpdateCS, threadCount);
			shaderProperties->SetStorageBuffer("instanceBuffer", m_pStorageBuffer.get());
		}
	}
	const std::string TestInstancedRendering::ToString() const
	{
		return "TestInstancedRendering";
	}
}