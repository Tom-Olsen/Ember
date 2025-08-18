#include "instancedRenderer.h"



namespace emberEngine
{
	// Constructor/Destructor:
	InstancedRenderer::InstancedRenderer()
	{
		m_instanceCount = 1;
		m_castShadows = true;
		m_receiveShadows = true;

		m_pMesh = nullptr;
		m_pMaterial = MaterialManager::GetMaterial("errorMaterial");
		m_pShaderProperties = std::make_unique<ShaderProperties>(m_pMaterial);
	}
	InstancedRenderer::~InstancedRenderer()
	{

	}



	// Public methods:
	// Setter:
	void InstancedRenderer::SetInstanceCount(uint32_t instanceCount)
	{
		m_instanceCount = instanceCount;
	}
	void InstancedRenderer::SetCastShadows(bool castShadows)
	{
		m_castShadows = castShadows;
	}
	void InstancedRenderer::SetReceiveShadows(bool receiveShadows)
	{
		m_receiveShadows = receiveShadows;
	}
	void InstancedRenderer::SetMesh(Mesh* pMesh)
	{
		m_pMesh = pMesh;
	}
	void InstancedRenderer::SetMaterial(Material* pMaterial)
	{
		if (pMaterial == nullptr)
		{
			if (m_pMaterial != MaterialManager::GetMaterial("errorMaterial"))
			{
				m_pMaterial = MaterialManager::GetMaterial("errorMaterial");
				m_pShaderProperties = std::make_unique<ShaderProperties>(m_pMaterial);
			}
		}
		else if (m_pMaterial != pMaterial)
		{
			m_pMaterial = pMaterial;
			m_pShaderProperties = std::make_unique<ShaderProperties>(m_pMaterial);
		}
	}
	void InstancedRenderer::SetInstanceBuffer(StorageBuffer* pInstanceBuffer)
	{
		m_pInstanceBuffer = pInstanceBuffer;
	}



	// Getters:
	uint32_t InstancedRenderer::GetInstanceCount() const
	{
		return math::Min(m_instanceCount, m_pInstanceBuffer->GetCount());
	}
	bool InstancedRenderer::GetCastShadows() const
	{
		return m_castShadows;
	}
	bool InstancedRenderer::GetReceiveShadows() const
	{
		return m_receiveShadows;
	}
	Mesh* InstancedRenderer::GetMesh()
	{
		return m_pMesh;
	}
	Material* InstancedRenderer::GetMaterial()
	{
		return m_pMaterial;
	}
	StorageBuffer* InstancedRenderer::GetInstanceBuffer()
	{
		return m_pInstanceBuffer;
	}
	ShaderProperties* InstancedRenderer::GetShaderProperties()
	{
		return m_pShaderProperties.get();
	}



	// Overrides:
	void InstancedRenderer::Update()
	{
		uint32_t instanceCount = math::Min(m_instanceCount, m_pInstanceBuffer->GetCount());
		Float4x4 localToWorldMatrix = m_pTransform->GetLocalToWorldMatrix();
		Graphics::DrawInstanced(instanceCount, m_pInstanceBuffer, m_pMesh, m_pMaterial, m_pShaderProperties.get(), localToWorldMatrix, m_receiveShadows, m_castShadows);
	}
	const std::string InstancedRenderer::ToString() const
	{
		return "InstancedRenderer";
	}
}