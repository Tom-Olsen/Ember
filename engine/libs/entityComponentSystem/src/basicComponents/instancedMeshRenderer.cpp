#include "instancedMeshRenderer.h"



namespace emberEngine
{
	// Constructor/Destructor:
	InstancedMeshRenderer::InstancedMeshRenderer()
	{
		m_instanceCount = 1;
		m_castShadows = true;
		m_receiveShadows = true;

		m_pMesh = nullptr;
		m_pMaterial = MaterialManager::TryGetMaterial("errorMaterial");
		m_shaderProperties = ShaderProperties(*m_pMaterial);
	}
	InstancedMeshRenderer::~InstancedMeshRenderer()
	{

	}



	// Public methods:
	// Setter:
	void InstancedMeshRenderer::SetInstanceCount(uint32_t instanceCount)
	{
		m_instanceCount = instanceCount;
	}
	void InstancedMeshRenderer::SetCastShadows(bool castShadows)
	{
		m_castShadows = castShadows;
	}
	void InstancedMeshRenderer::SetReceiveShadows(bool receiveShadows)
	{
		m_receiveShadows = receiveShadows;
	}
	void InstancedMeshRenderer::SetMesh(Mesh& mesh)
	{
		m_pMesh = &mesh;
	}
	void InstancedMeshRenderer::SetMaterial(Material& material)
	{
		if (m_pMaterial->GetName() != material.GetName())
		{
			m_pMaterial = &material;
			m_shaderProperties = ShaderProperties(material);
		}
	}
	void InstancedMeshRenderer::SetInstanceBuffer(Buffer& instanceBuffer)
	{
		m_pInstanceBuffer = &instanceBuffer;
	}



	// Getters:
	uint32_t InstancedMeshRenderer::GetInstanceCount() const
	{
		return math::Min(m_instanceCount, m_pInstanceBuffer->GetCount());
	}
	bool InstancedMeshRenderer::GetCastShadows() const
	{
		return m_castShadows;
	}
	bool InstancedMeshRenderer::GetReceiveShadows() const
	{
		return m_receiveShadows;
	}
	Mesh& InstancedMeshRenderer::GetMesh()
	{
		return *m_pMesh;
	}
	Material& InstancedMeshRenderer::GetMaterial()
	{
		return *m_pMaterial;
	}
	Buffer& InstancedMeshRenderer::GetInstanceBuffer()
	{
		return *m_pInstanceBuffer;
	}
	ShaderProperties& InstancedMeshRenderer::GetShaderProperties()
	{
		return m_shaderProperties;
	}



	// Overrides:
	void InstancedMeshRenderer::Update()
	{
		uint32_t instanceCount = math::Min(m_instanceCount, m_pInstanceBuffer->GetCount());
		Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
		Renderer::DrawInstanced(instanceCount, *m_pInstanceBuffer, *m_pMesh, *m_pMaterial, m_shaderProperties, localToWorldMatrix, m_receiveShadows, m_castShadows);
	}
}