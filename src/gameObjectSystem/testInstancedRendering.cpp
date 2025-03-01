#include "testInstancedRendering.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Constructor/Destructor:
	TestInstancedRendering::TestInstancedRendering(VulkanContext* pVulkanContext, uint32_t instanceCount)
	{
		m_pVulkanContext = pVulkanContext;
		uint32_t elementSize = sizeof(Float4x4) + sizeof(Float4);
		m_pStorageBuffer = std::make_unique<StorageBuffer>(m_pVulkanContext, instanceCount, elementSize);
		m_pStartCS = ComputeShaderManager::GetComputeShader("initialPositionsComputeShader");
		m_pUpdateCS = ComputeShaderManager::GetComputeShader("updatePositionsComputeShader");
		m_pStartProperties = std::make_unique<ShaderProperties>((Shader*)m_pStartCS);
		m_pUpdateProperties = std::make_unique<ShaderProperties>((Shader*)m_pUpdateCS);
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
			m_pStartProperties->SetStorageBuffer("instanceBuffer", m_pStorageBuffer.get());
			Compute::Dispatch(m_pStartCS, m_pStartProperties.get(), threadCount);
			Compute::Barrier(ComputeStageAccessMask::shaderWrite, ComputeStageAccessMask::shaderRead);
		}
	}
	void TestInstancedRendering::Update()
	{
		if (m_pUpdateCS != nullptr)
		{
			Uint3 threadCount = Uint3(m_pStorageBuffer->GetCount(), 1, 1);
			m_pUpdateProperties->SetStorageBuffer("instanceBuffer", m_pStorageBuffer.get());
			Compute::Dispatch(m_pUpdateCS, m_pUpdateProperties.get(), threadCount);
		}
	}
	const std::string TestInstancedRendering::ToString() const
	{
		return "TestInstancedRendering";
	}
}